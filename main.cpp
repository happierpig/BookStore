#include <cstring>
#include <iostream>
#include <fstream>
#include "BookStore.h"
#include "programm.h"
#include "UnrolledLinkedList.hpp"
using namespace std;
int main() {
    initialize();
//    for(int i = 1;i <= 10;++i) {
//        char name[6];
//        name[0] = i + '0';name[1] = '.';name[2] = 'i';name[3] ='n';name[4] = 0;
//        fstream fin(name);
//        string token;
//        while (getline(fin, token)) {
//            try {
//                parseStatement(token);
//            } catch (int &x) {
//                if (x == 1) { return 0; }
//            }
//            catch (...) {
//                cout << "Invalid" << endl;
//            }
//        }
//    }
    string token;
    while (getline(cin, token)) {
        try {
            parseStatement(token);
        } catch (int &x) {
            if (x == 1) { return 0; }
        }
        catch (...) {
            cout << "Invalid" << endl;
        }
    }
}
