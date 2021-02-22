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
    const int MergeBoundary = 300;

    template<int KeyLength>
    class UnrolledLinkedList {
    public:
        struct Element{
            friend UnrolledLinkedList<KeyLength>;
            char key[KeyLength+1]; int MemPos; //该key所 对应的内容在文件中的位置
            Element(){ MemPos = -1; memset(key,'0',sizeof(key));}

            Element(std::string o,int x):MemPos(x){
                strcpy(this->key,o.c_str());
            }

            Element(const Element & o){
                strcpy(this->key,o.key);
                this->MemPos = o.MemPos;
            }

            bool operator < (const Element & o) const{
                return strcmp(this->key,o.key) < 0;
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
            int EleNum,NexPtr,BePtr; //指针指向下一个块
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

        int NextBlock(const int position){
            OrinFile.seekp(position + sizeof(int));
            int next = -1;OrinFile.read(r_cast(next),sizeof(int));
            return next;
        }

        void MergeBlock(int position){
            if(!OrinFile.is_open()){OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);}
            Block first,second;
            OrinFile.seekp(position);OrinFile.read(r_cast(first),BlockSize);
            OrinFile.seekp(NextBlock(position));OrinFile.read(r_cast(second),BlockSize);

            first.NexPtr = second.NexPtr;first.EleNum += second.EleNum;
            if(second.NexPtr != -1){
                OrinFile.seekp(second.NexPtr + sizeof(int) * 2);
                OrinFile.write(r_cast(position),sizeof(int));
            }
            for(int i = first.EleNum;i < first.EleNum + second.EleNum;i++){
                first.content[i] = second.content[i-first.EleNum];
            }
            OrinFile.seekp(position);OrinFile.write(r_cast(first),BlockSize);
            OrinFile.close();
        }

        void SplitBlock(const int position){
            if(!OrinFile.is_open()){OrinFile.open(OrinFileName,std::fstream::binary | std::fstream::in | std::fstream::out);}
            Block origin,temp;
            OrinFile.seekp(position);
            OrinFile.read(r_cast(origin),BlockSize);
            OrinFile.seekp(0,std::ios::end); int temppos = OrinFile.tellp();
            temp.EleNum = MaxEleNum - HalfMax; origin.EleNum = HalfMax;
            temp.NexPtr = origin.NexPtr; origin.NexPtr = temppos;
            temp.BePtr = position;
            if(temp.NexPtr != -1){
                OrinFile.seekp(temp.NexPtr + 2 * sizeof(int));
                OrinFile.write(r_cast(temppos),sizeof(int));
            }
            for(int i = 0 ; i < temp.EleNum;++i){
                temp.content[i] = origin.content[origin.EleNum + i];
            }

            OrinFile.seekp(position);OrinFile.write(r_cast(origin),BlockSize);
            OrinFile.seekp(temppos);OrinFile.write(r_cast(temp),BlockSize);
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
                OrinFile.write(r_cast(temp),BlockSize);
                return;
            }
            count1 = 0; count2 = NextBlock(count1);
            if(count2 == -1){
                OrinFile.seekp(count1);OrinFile.read(r_cast(temp),BlockSize);
                if(temp.EleNum == 0){
                    temp.EleNum++;
                    temp.content[0] = ele;
                    OrinFile.seekp(count1);OrinFile.write(r_cast(temp),BlockSize);
                    return;
                }
            }
            while(count2 != -1){
                OrinFile.seekp(count2 + sizeof(int) * 3);
                OrinFile.read(r_cast(ind),ElementSize);
                if(ele < ind){
                    break;
                }else{
                    count1 = count2;
                    count2 = NextBlock(count1);
                    continue;
                }
            }
            OrinFile.seekp(count1);OrinFile.read(r_cast(temp),BlockSize);
            count2 = 0;
            while(true){
                if(ele < temp.content[count2]) break;
                count2++;
                if(count2 == temp.EleNum) break;
            }
            for(int i = temp.EleNum;i > count2;i--){
                temp.content[i] = temp.content[i-1];
            }
            temp.content[count2] = ele;temp.EleNum++;
            OrinFile.seekp(count1);OrinFile.write(r_cast(temp),BlockSize);
            if(temp.EleNum == MaxEleNum){
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
                OrinFile.seekp(count2 + sizeof(int) * 3);
                OrinFile.read(r_cast(ind),ElementSize);
                if(ele < ind){
                    break;
                }else{
                    count1 = count2;
                    count2 = NextBlock(count1);
                    continue;
                }
            }
            OrinFile.seekp(count1);OrinFile.read(r_cast(temp),BlockSize);
            count2 = 0;
            while(true){
                if(temp.content[count2] == ele) break;
                count2++;
                if(count2 == temp.EleNum) break;
            }
            if(count2 == temp.EleNum - 1) temp.content[count2] = Element();
            for(int i = count2;i < temp.EleNum - 1;i++){
                temp.content[i] = temp.content[i+1];
            }
            temp.EleNum--;
            OrinFile.seekp(count1);OrinFile.write(r_cast(temp),BlockSize);
            if(temp.NexPtr != -1){
                OrinFile.seekp(temp.NexPtr);int nextNum;
                OrinFile.read(r_cast(nextNum),sizeof(int));
                if(temp.EleNum + nextNum < MergeBoundary) MergeBlock(count1);
            }else if(temp.EleNum == 0 && temp.BePtr != -1){
                OrinFile.seekp(temp.BePtr + 2 * sizeof(int));
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
            if(!count1){
                return;
            }
            count1 = 0; count2 = NextBlock(count1);
            while(count1 != -1){
                OrinFile.seekp(count1);
                OrinFile.read(r_cast(temp),BlockSize);
                if(strcmp(temp.content[temp.EleNum-1].key,targetKey.c_str()) < 0){
                    count1 = count2;count2 = NextBlock(count1);
                    continue;
                }
                if(strcmp(targetKey.c_str(),temp.content[0].key) < 0) return;
                for(int i = 0;i < temp.EleNum;i++){
                    if(strcmp(temp.content[i].key,targetKey.c_str()) == 0) vec_ans.push_back(temp.content[i].MemPos);
                }
                count1 = count2;count2 = NextBlock(count1);
            }
            OrinFile.close();
        }

        void PrintULL(std::vector<int> & vec_ans){
//            OrinFile.seekp(0,std::ios::end);int count1,count2 = 0;
//            count1 = OrinFile.tellp();
//            Block temp;
//            if(!count1){
//                cout << "-----Total Block Number : 0" << endl;
//                return;
//            }
//            count1 = 0;
//            while(count1 != -1){
//                count2++;
//                OrinFile.seekp(count1);
//                OrinFile.read(r_cast(temp),BlockSize);
//                cout << "-----Block Number : " << count2 << endl;
//                for(int i = 0;i < temp.EleNum;++i){
//                    cout << i << " Key : " << temp.content[i].key << endl;
//                    cout << i << " Pos : " << temp.content[i].MemPos << endl;
//                }
//                count1 = NextBlock(count1);
//            }
//            cout << "-----Total Block Number : " << count2 << endl;
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