//
// Created by 赵先生 on 2021/2/21.
//

#ifndef SRC_USER_H
#define SRC_USER_H

#include <iostream>
#include <fstream>
#include "UnrolledLinkedList.hpp"
#include "BPlusTree.hpp"
#include <cstring>
#include <vector>
#include <stack>
#include <sstream>
#include <map>

using namespace std;
extern stack<pair<string,int>> OnlineUser;
//extern ULL::UnrolledLinkedList<30> userID_ULL; // userData.txt
//extern ULL::UnrolledLinkedList<20> ISBN_ULL; // BookData.txt
//extern ULL::UnrolledLinkedList<60> name_ULL;
//extern ULL::UnrolledLinkedList<60> author_ULL;
//extern ULL::UnrolledLinkedList<60> keyword_ULL;
//extern ULL::UnrolledLinkedList<30> log_ULL;

extern BPlusTree<Key,int> ISBN_ULL;
extern BPlusTree<Key,int> name_ULL;
extern BPlusTree<Key,int> author_ULL;
extern BPlusTree<Key,int> keyword_ULL;
extern BPlusTree<Key,int> userID_ULL;
extern BPlusTree<Key,int> log_ULL;

struct User{
        int privilege = 0;
        char userID[45] = {0};
        char passwd[45] = {0};
        char name[45] = {0};
        User() = default;
        User(const int _privilege,string _userID,string _passwd,string _name);
    };
User getUser(string _userID);
void registerUser(string _userID,string _passwd,string _name);
void su(string _userID,string _passwd);
void logout();
void addUser(string _userID,string _passwd,int _privilege,string _name);
void deleteUser(string _userID);
void changePasswd(string _userID,string _oldpasswd,string _newpasswd);

struct Book{
        char ISBN[35] = {0};
        char name[75] = {0};
        char author[75] = {0};
        char keyword[75] = {0};
        int quantity = 0;
        double price = 0.00;
        Book() = default;
        Book(string _isbn){
            strcpy(ISBN,_isbn.c_str());
        }
        bool operator < (const Book & o) const{
            return strcmp(this->ISBN,o.ISBN) < 0;
        }
        friend ostream & operator << (ostream & os,const Book & book){
            os << book.ISBN << '\t' << book.name << '\t' << book.author << '\t' << book.keyword << '\t';
            printf("%0.2f",book.price);
            os << '\t' << book.quantity;
            return os;
        }
    };
bool isEmpty(string _isbn);
pair<Book,int> getBook(string _isbn);
void select(string _isbn);
void importBook(int _quantity,double _cost);
void buyBook(string _isbn,int _quantity);
void modifyBook(string mode,string token);
void showBook(int mode,string token = "");
void manageMoney(double number,bool mode);
void showFinance(int times);
void report(int mode);
#endif //SRC_USER_H
