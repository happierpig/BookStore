#include <cstring>
#include <iostream>
#include "BookStore.h"
#include "programm.h"
#include "UnrolledLinkedList.hpp"

using namespace std;
int main() {
    initialize();
//     fstream fin("5.txt");
    string token;
    while(getline(cin,token)){
        try {
            parseStatement(token);
        }catch (int){
            return 0;
        }
        catch (...) {
            cout << token << endl;
            cout << "Invalid" << endl;
        }
    }
}
