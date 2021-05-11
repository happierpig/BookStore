#include <cstring>
#include <iostream>
#include <fstream>
#include "BookStore.h"
#include "programm.h"
#include "UnrolledLinkedList.hpp"
using namespace std;
int main() {
    initialize();
//    fstream fin("10.in");
    string token;
    while(getline(cin,token)){
        try {
            parseStatement(token);
        }catch (int &x){
            if(x == 1){return 0;}
        }
        catch (...) {
            cout << "Invalid" << endl;
        }
    }
}
