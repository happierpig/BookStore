#include <cstring>
#include <iostream>
#include <fstream>
#include "BookStore.h"
#include "programm.h"
#include "UnrolledLinkedList.hpp"

using namespace std;
int main() {
    initialize();
//    fstream fin("5.in");
    string token;
    while(getline(cin,token)){
        try {
            parseStatement(token);
        }catch (int &x){
            if(x == 1){return 0;}
            else{
                fstream file;file.open("logData.txt",ios::out | ios::in | ios::binary);
                file.seekg(0,ios::end);int MemPos = file.tellg();
                log_ULL.addElement(ULL::UnrolledLinkedList<30>::Element(OnlineUser.top().first,MemPos));
                token += "\t" + OnlineUser.top().first;
                char tempToken[60];strcpy(tempToken,token.c_str());
                file.write(r_cast(tempToken),sizeof(tempToken));
                file.close();
            }
        }
        catch (...) {
            cout << "Invalid" << endl;
        }
    }
}
