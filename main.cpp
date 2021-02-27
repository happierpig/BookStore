#include <cstring>
#include <iostream>
#include "BookStore.h"
#include "programm.h"
#include "UnrolledLinkedList.hpp"

using namespace std;
int main() {
    initialize();
    fstream fin("5.in");
    string token;
    while(getline(fin,token)){
        try {
            parseStatement(token);
        }catch (int){
            return 0;
        }
        catch (...) {
            cout << token << endl;
//            cout << "Invalid" << endl;
        }
    }
}
