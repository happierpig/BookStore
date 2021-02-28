#ifndef HAPPYPIG_ULL_HPP
#define HAPPYPIG_ULL_HPP

#include <fstream>
#include <cstring>
#include <vector>
#include <iostream>

#define r_cast(x) reinterpret_cast<char *>(&x)
using std::cout;
using std::endl;
namespace ULL {

    const int MaxEleNum = 400;
    const int HalfMax = 200;
    const int MergeBoundary = 50;

    template<int KeyLength>
    class UnrolledLinkedList {
    public:
        struct Element{
            char key[KeyLength+1]; int MemPos; //该key所   对应的内容在文件中的位置
            Element(){ MemPos = -1; memset(key,0,sizeof(key));}

            Element(std::string o,int x):MemPos(x){
                strcpy(this->key,o.c_str());
            }

            Element(const Element & o){
                strcpy(this->key,o.key);
                this->MemPos = o.MemPos;
            }

            bool operator < (const Element & o) const{
                if(strcmp(this->key,o.key) < 0){
                    return true;
                }else if(strcmp(this->key,o.key) > 0){
                    return false;
                }else return this->MemPos < o.MemPos;
            }

            bool operator == (const Element & o ) const {
                return ( strcmp(this->key,o.key) == 0 && (this->MemPos == o.MemPos) );
            }

            Element & operator=(const Element & o){
                if(this == &o) return *this;
                this->MemPos = o.MemPos;
                strcpy(this->key,o.key);
                return *this;
            }

        };

    private:
        class Block{
            friend UnrolledLinkedList<KeyLength>;
        private:
            int EleNum,NexPtr,BePtr;
        public:
            Element content[MaxEleNum];
            Block():EleNum(0),NexPtr(-1),BePtr(-1){}
        };

        std::string OrinFileName;
        std::fstream OrinFile;
    public:
        const int BlockSize = sizeof(Block);
        const int ElementSize = sizeof(Element);
    private:

        int NextBlock(const int position){ // return memory pointer to next block;
            if(!OrinFile.is_open()) {OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);}
            OrinFile.seekp(position + sizeof(int),std::ios::beg);
            int next = -1;OrinFile.read(r_cast(next),sizeof(int));
            return next;
        }

        void MergeBlock(int position){ // merge block in position with block in NextBlock(position)
            if(!OrinFile.is_open()){OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);}
            Block first,second;
            OrinFile.seekp(position,std::ios::beg);OrinFile.read(r_cast(first),BlockSize);
            OrinFile.seekp(NextBlock(position),std::ios::beg);OrinFile.read(r_cast(second),BlockSize);
            // 1<->2<->3  -->>  1<->3
            first.NexPtr = second.NexPtr;first.EleNum += second.EleNum;
            if(second.NexPtr != -1){
                OrinFile.seekp(second.NexPtr + sizeof(int) * 2,std::ios::beg);
                OrinFile.write(r_cast(position),sizeof(int));
            }
            for(int i = first.EleNum;i < first.EleNum + second.EleNum;i++){
                first.content[i] = second.content[i-first.EleNum];
            }
            OrinFile.seekp(position,std::ios::beg);
            OrinFile.write(r_cast(first),BlockSize);
            OrinFile.close();
        }

        void SplitBlock(int position){ // split block in position into block:: origin  and temp
            if(!OrinFile.is_open()){OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);}
            Block origin,temp;
            OrinFile.seekp(position,std::ios::beg);
            OrinFile.read(r_cast(origin),BlockSize);
            OrinFile.seekp(0,std::ios::end); int temppos = OrinFile.tellp(); // find the new memory location
            temp.EleNum = origin.EleNum - HalfMax; origin.EleNum = HalfMax;
            // 1<->3  --->>> 1<->2<->3
            temp.NexPtr = origin.NexPtr; origin.NexPtr = temppos;
            temp.BePtr = position;
            if(temp.NexPtr != -1){
                OrinFile.seekp(temp.NexPtr + 2 * sizeof(int),std::ios::beg);
                OrinFile.write(r_cast(temppos),sizeof(int));
            }
            for(int i = 0 ; i < temp.EleNum;++i){
                temp.content[i] = origin.content[origin.EleNum + i];
            }
            OrinFile.seekp(position,std::ios::beg);OrinFile.write(r_cast(origin),BlockSize);
            OrinFile.seekp(temppos,std::ios::beg);OrinFile.write(r_cast(temp),BlockSize);
            OrinFile.close();
        }
    public:
        explicit UnrolledLinkedList(std::string name):OrinFileName(name){
            OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);
            if(!OrinFile){
                OrinFile.open(OrinFileName,std::fstream::out);OrinFile.close();
                OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);
            }
        }
        ~UnrolledLinkedList(){
            if(OrinFile.is_open()){OrinFile.close();}
        }
        void addElement(const Element & ele){
            if(!OrinFile.is_open()){OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);}
            OrinFile.seekp(0,std::ios::end);int count1,count2;
            count1 = OrinFile.tellp();
            Block temp;Element ind;
            if(!count1){
                temp.content[temp.EleNum++] = ele;
                OrinFile.seekp(0,std::ios::beg);
                OrinFile.write(r_cast(temp),BlockSize);
                OrinFile.close();
                return;
            }

            count1 = 0; count2 = NextBlock(count1);
            if(count2 == -1){
                OrinFile.seekp(count1,std::ios::beg);OrinFile.read(r_cast(temp),BlockSize);
                if(temp.EleNum == 0){
                    temp.EleNum++;
                    temp.content[0] = ele;
                    OrinFile.seekp(0,std::ios::beg);OrinFile.write(r_cast(temp),BlockSize);
                    OrinFile.close();
                    return;
                }
            }
            while(count2 != -1){
                OrinFile.seekp(count2 + sizeof(int) * 3,std::ios::beg);
                OrinFile.read(r_cast(ind),ElementSize);
                if(ele < ind){
                    break;
                }else{
                    count1 = count2;
                    count2 = NextBlock(count2);
                    continue;
                }
            }
            OrinFile.seekp(count1,std::ios::beg);OrinFile.read(r_cast(temp),BlockSize);
            count2 = 0;
            while(true){
                if(ele < temp.content[count2]) break;
                count2++;
                if(count2 == temp.EleNum) break;
            }
            for(int i = temp.EleNum;i > count2;i--){
                temp.content[i] = temp.content[i-1];
            }
            temp.content[count2] = ele;
            temp.EleNum++;
            OrinFile.seekp(count1,std::ios::beg);OrinFile.write(r_cast(temp),BlockSize);
            if(temp.EleNum >= MaxEleNum){
                SplitBlock(count1);
            }
            OrinFile.close();
        }

        void DeleteElement(const Element & ele){
            if(!OrinFile.is_open()){OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);}
            OrinFile.seekp(0,std::ios::end);int count1,count2;
            count1 = OrinFile.tellp();
            Block temp;Element ind;
            if(!count1){
                return;
            }
            count1 = 0; count2 = NextBlock(count1);
            while(count2 != -1){
                OrinFile.seekp(count2 + sizeof(int) * 3,std::ios::beg);
                OrinFile.read(r_cast(ind),ElementSize);
                if(ele < ind){
                    break;
                }else{
                    count1 = count2;
                    count2 = NextBlock(count2);
                    continue;
                }
            }
            OrinFile.seekp(count1,std::ios::beg);OrinFile.read(r_cast(temp),BlockSize);
            count2 = 0;
            while(true){
                if(temp.content[count2] == ele) break;
                count2++;
                if(count2 == temp.EleNum) return;
            }
            for(int i = count2;i < temp.EleNum - 1;i++){
                temp.content[i] = temp.content[i+1];
            }
            temp.content[temp.EleNum-1] = Element();
            temp.EleNum--;
            OrinFile.seekp(count1,std::ios::beg);OrinFile.write(r_cast(temp),BlockSize);
            if(temp.NexPtr != -1 && temp.EleNum < MergeBoundary){
                MergeBlock(count1);
            }else if(temp.EleNum == 0 && temp.BePtr != -1){
                OrinFile.seekp(temp.BePtr + sizeof(int),std::ios::beg);
                int k = -1;
                OrinFile.write(r_cast(k),sizeof(int));
            }
            OrinFile.close();
        }
        void FindOffset(std::string targetKey,std::vector<int> & vec_ans){
            if(!OrinFile.is_open()){OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);}
            OrinFile.seekp(0,std::ios::end);int count1,count2;
            count1 = OrinFile.tellp();
            Block temp;
            if(!count1){return;}
            count1 = 0; count2 = NextBlock(count1);
            while(count1 != -1){
                OrinFile.seekp(count1,std::ios::beg);
                OrinFile.read(r_cast(temp),BlockSize);
                if(strcmp(temp.content[temp.EleNum-1].key,targetKey.c_str()) < 0){
                    count1 = count2;count2 = NextBlock(count2);
                    continue;
                }
                if(strcmp(targetKey.c_str(),temp.content[0].key) < 0) return;
                for(int i = 0;i < temp.EleNum;i++){
                    if(strcmp(temp.content[i].key,targetKey.c_str()) == 0) vec_ans.push_back(temp.content[i].MemPos);
                }
                count1 = count2;count2 = NextBlock(count2);
            }
            OrinFile.close();
        }

        void PrintULL(std::vector<int> & vec_ans){
            if(!OrinFile.is_open()){OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);}
            OrinFile.seekp(0);
            int count = 0;
            Block temp;
            while(count != -1){
                OrinFile.seekp(count);
                OrinFile.read(r_cast(temp),BlockSize);
                for(int i = 0;i < temp.EleNum;i++){
                    vec_ans.push_back(temp.content[i].MemPos);
                }
                count = NextBlock(count);
              }
            OrinFile.close();
        }
    };
}
#endif //HAPPYPIG_ULL_HPP