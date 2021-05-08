//
// Created by 赵先生 on 2021/2/21.
//
#ifndef SRC_PROGRAMM_H
#define SRC_PROGRAMM_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "BookStore.h"
#include "BPlusTree.hpp"
using namespace std;

//ULL::UnrolledLinkedList<20> ISBN_ULL("ISBN_ULL.dat");
//ULL::UnrolledLinkedList<60> name_ULL("name_ULL.dat");
//ULL::UnrolledLinkedList<60> author_ULL("author_ULL.dat");
//ULL::UnrolledLinkedList<60> keyword_ULL("keyword_ULL.dat");
//ULL::UnrolledLinkedList<30> userID_ULL("userID_ULL.dat");
//ULL::UnrolledLinkedList<30> log_ULL("log_ULL.dat");
BPlusTree<Key,int> ISBN_ULL("ISBN");
BPlusTree<Key,int> name_ULL("name_ULL");
BPlusTree<Key,int> author_ULL("author_ULL");
BPlusTree<Key,int> keyword_ULL("keyword_ULL");
BPlusTree<Key,int> userID_ULL("userID_ULL");
BPlusTree<Key,int> log_ULL("log_ULL");

stack<pair<string,int>> OnlineUser;

string deleteQuo(string origin){
    if(origin[0] == '"') {
        string temp = origin;
        for (int i = 0; i < origin.size() - 2; ++i) temp[i] = origin[i + 1];
        temp[origin.size() - 2] = '\0';temp[origin.size() - 1] = '\0';temp[origin.size()] = '\0';
        return temp;
    }else{
        return origin;
    }
}

void parseStatement(string order){
    if(order.empty()) return;
    stringstream sentence;sentence.clear();sentence.str(order);
    int tokencount = 1;

    for(int i = 0;i < order.size();i++){
        if(order[i] == ' ') tokencount++;
    }

    for(int i = order.size()-1;i>=0;i--){
        if(order[i] == ' ' ){
            tokencount--;
            continue;
        }
        break;
    }

    string token1,token2,token3;
    sentence >> token1;
    if(token1 == "su"){
        if(tokencount == 2){
            sentence >> token2;
            su(token2.c_str(),"");
            return;
        }
        if(tokencount == 3){
            sentence >> token2 >> token3;
            su(token2.c_str(),token3.c_str());
            return;
        }
        throw "Invalid";
    }
    if(token1 == "logout"){
        if(tokencount > 1) throw "Invalid";
        logout();
        return;
    }
    if(token1 == "useradd"){
        if(tokencount != 5) throw "Invalid";
        int _prority = 0;
        sentence >> token1 >> token2 >> _prority >> token3;
        if( _prority != 3 && _prority != 1) throw "Invalid";
        addUser(token1.c_str(),token2.c_str(),_prority,token3.c_str());
        return;
    }
    if(token1 == "register"){
        if(tokencount != 4) throw "Invalid";
        sentence >> token1 >> token2 >> token3;
        registerUser(token1.c_str(),token2.c_str(),token3.c_str());
        return;
    }
    if(token1 == "delete"){
        if(tokencount != 2) throw "Invalid";
        sentence >> token1;
        deleteUser(token1.c_str());
        return;
    }
    if(token1 == "passwd"){
        if(tokencount == 3){
            sentence >> token1 >> token2;
            changePasswd(token1,"[wrong]",token2);
            return;
        }else if(tokencount == 4){
            sentence >> token1 >> token2 >> token3;
            changePasswd(token1,token2,token3);
            return;
        }
        throw "Invalid";
    }
    if(token1 == "select"){
        if(tokencount != 2) throw "Invalid";
        sentence >> token1;
        select(token1);
        return;
    }
    if(token1 == "import"){
        if(tokencount != 3) throw "Invalid";
        int _quantity = -1;double _cost = -1;
        sentence >> _quantity >> _cost;
        if(_quantity == -1 || _cost == -1) throw "Invalid";
        importBook(_quantity,_cost);
        return;
    }
    if(token1 == "buy"){
        if(tokencount != 3) throw "Invalid";
        int _quantity = -1;
        sentence >> token1 >> _quantity;
        if(_quantity == -1) throw "Invalid";
        buyBook(token1,_quantity);
        return;
    }
    if(token1 == "modify"){
        while(--tokencount){
            char temp[10];
            sentence.get();
            sentence.getline(temp,10,'=');
            sentence >> token1;
#ifdef debug
            if(token1 == "\"briar|telephone|keyword\""){
                cout << 1;
            }
#endif
            modifyBook(temp,deleteQuo(token1));
        }
        return;
    }
    if(token1 == "show"){
        if(tokencount == 1){
            showBook(1);
            return;
        }else{
            stringstream sentence2(order);
            sentence2 >> token1;sentence2 >> token1;
            if(token1 == "finance"){
                if(tokencount == 2){
                    showFinance(0);
                    return;
                }
                if(tokencount == 3){
                    int x = -1;sentence2 >> x;
                    if(x <= 0) throw "invalid";
                    showFinance(x);
                    return;
                }
                throw "invalid";
            }
            char temp[10] = {0};
            sentence.get();
            sentence.getline(temp,10,'=');
            sentence >> token1;
            if(strcmp(temp,"-ISBN") == 0){
                showBook(2,token1);
                return;
            }else if(strcmp(temp,"-name") == 0){
                showBook(3,deleteQuo(token1));
                return;
            }else if(strcmp(temp,"-author") == 0){
                showBook(4,deleteQuo(token1));
                return;
            }else if(strcmp(temp,"-keyword") == 0){
                showBook(5,deleteQuo(token1));
                return;
            }
            throw "invalid";
        }
    }
    if(token1 == "quit" || token1 == "exit"){
        throw int(1);
    }
    if(token1 == "log"){
        report(1);
        return;
    }
    if(token1 == "report"){
        sentence >> token2;
        if(token2 == "myself"){
            report(2);
        }
        return;
    }
    throw "invalid";
}

void initialize(){
    fstream file;file.open("userData.txt",fstream::binary | fstream::in | fstream::out);
    if(!file){
        file.open("userData.txt",fstream::out);file.close();
        file.open("BookData.txt",fstream::out);file.close();
        file.open("FinanceData.txt",fstream::out);file.close();
        file.open("logData.txt",fstream::out);file.close();

        file.open("FinanceData.txt",fstream::binary | fstream::in | fstream::out);
        pair<double,double> finance(0,0);int counter = 0;
        file.write(r_cast(counter),sizeof(int));file.seekp(sizeof(int),ios::beg);
        file.write(r_cast(finance),sizeof(pair<double,double>));file.close();

        User temp(7,"root","sjtu","root");
        file.open("userData.txt",fstream::binary | fstream::in | fstream::out);
        file.write(r_cast(temp),sizeof(User));file.close();
        userID_ULL.insert(Key("root"),0);
    }
    if(file.is_open()) file.close();
}

#endif //SRC_PROGRAMM_H
