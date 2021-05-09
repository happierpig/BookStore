#include <cstring>
#include <iostream>
#include <fstream>
#include "BookStore.h"
#include "programm.h"
#include "UnrolledLinkedList.hpp"
using namespace std;
int main() {
    initialize();
    fstream fin("1.txt");
    string token;
    while(getline(fin,token)){
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
