//
// Created by 赵先生 on 2021/2/21.
//
#include "BookStore.h"

User::User(const int _privilege, string _userID, string _passwd, string _name):privilege(_privilege){
    strcpy(userID,_userID.c_str());strcpy(passwd,_passwd.c_str());strcpy(name,_name.c_str());
}

void manageMoney(double number,bool mode){ // true for ben  efit ; false for cost
    fstream file;file.open("FinanceData.txt",fstream::binary | fstream::in | fstream::out);
    int counter;pair<double,double> finance;
    file.read(r_cast(counter),sizeof(int));file.read(r_cast(finance),sizeof(pair<double,double>));
    counter++;
    if(mode){
        finance.first += number;
        file.seekp(0,ios::end);file.write(r_cast(number),sizeof(double));
    }else{
        finance.second += number; number = -number;
        file.seekp(0,ios::end);file.write(r_cast(number),sizeof(double));
    };
    file.seekp(0,ios::beg);file.write(r_cast(counter),sizeof(int));file.write(r_cast(finance),sizeof(pair<double,double>));
    file.close();
}

void registerUser(string _userID,string _passwd,string _name){
    vector<int> vec_ans;userID_ULL.FindOffset(_userID,vec_ans);
    if(!vec_ans.empty()) throw "repeated register";
    User temp(1,_userID,_passwd,_name);
    fstream file;file.open("userData.txt",fstream::binary | fstream::in | fstream::out | fstream::ate);
    int MemPos = file.tellp();file.write(r_cast(temp),sizeof(User));file.close();
    userID_ULL.addElement(ULL::UnrolledLinkedList<30>::Element(_userID,MemPos));
}

User getUser(string _userID){
    vector<int> vec_ans;userID_ULL.FindOffset(_userID,vec_ans);
    int position = vec_ans[0];
    fstream file;file.open("userData.txt",fstream::binary | fstream::in | fstream::out);
    file.seekp(position);
    User temp;file.read(r_cast(temp),sizeof(User));
    file.close();
    return temp;
}

void su(string _userID,string _passwd){
//    if (_userID=="vE0iDoWcMu"){
//        cout << "check" << endl ;
//    }
    vector<int> vec_ans;userID_ULL.FindOffset(_userID,vec_ans);
    if(vec_ans.empty()) throw "blank account";
    User logging = getUser(_userID);
    if(!OnlineUser.empty()) {
        User online = getUser(OnlineUser.top().first);
        if (online.privilege > logging.privilege && strcmp(_passwd.c_str(),"") == 0) {
            OnlineUser.push(make_pair(_userID,-1));
            return;
        }
        if (strcmp(_passwd.c_str(), logging.passwd) == 0) {
            OnlineUser.push(make_pair(_userID,-1));
            return;
        }
        throw "Wrong passwd";
    }else{
        if (strcmp(_passwd.c_str(), logging.passwd) == 0) {
            OnlineUser.push(make_pair(_userID,-1));
            return;
        }
        throw "wrong passwd";
    }
}

void logout(){
    if(OnlineUser.empty()) throw "unlogged";
    OnlineUser.pop();
}

void addUser(string _userID,string _passwd,int _privilege,string _name){ // privilege 7/3/1
    vector<int> vec_ans;userID_ULL.FindOffset(_userID,vec_ans);
    if(!vec_ans.empty()) throw "repeated register";
    if(OnlineUser.empty()) throw "unlogged";
    User online = getUser(OnlineUser.top().first);
    if(online.privilege <= _privilege) throw "low privilege";
    User temp(_privilege,_userID,_passwd,_name);
    fstream file;file.open("userData.txt",fstream::binary | fstream::in | fstream::out | fstream::ate);
    file.seekp(0,std::ios::end);
    int MemPos = file.tellp();file.write(r_cast(temp),sizeof(User));
    file.close();
    userID_ULL.addElement(ULL::UnrolledLinkedList<30>::Element(_userID,MemPos));
    throw int(2);
}

void deleteUser(string _userID){
    vector<int> vec_ans;userID_ULL.FindOffset(_userID,vec_ans);
    if(OnlineUser.empty()) throw "unlogged";
    if(vec_ans.empty()) throw "delete blank account";
    User online = getUser(OnlineUser.top().first);
    if(online.privilege != 7) throw "Invaild";
    if(vec_ans[0] == 0) throw "delete root";
    stack<pair<string,int>> check_OnlineUser(OnlineUser);
    while(!check_OnlineUser.empty()){
        if(check_OnlineUser.top().first == _userID) throw "invalid";
        check_OnlineUser.pop();
    }
    userID_ULL.DeleteElement(ULL::UnrolledLinkedList<30>::Element(_userID,vec_ans[0]));
    throw int(2);
}

void changePasswd(string _userID,string _oldpasswd,string _newpasswd){
    if(OnlineUser.empty()) throw "unlogged";
    User online = getUser(OnlineUser.top().first);
    vector<int> vec_ans;userID_ULL.FindOffset(_userID,vec_ans);
    if(vec_ans.empty()) throw "404 not found";
    User temp = getUser(_userID);
    if(online.privilege == 7){
        if(_oldpasswd == "[wrong]"){strcpy(temp.passwd,_newpasswd.c_str());}
        else if(strcmp(_oldpasswd.c_str(),temp.passwd) == 0){strcpy(temp.passwd,_newpasswd.c_str());}
        else throw "invalid";
    }else if(strcmp(_oldpasswd.c_str(),temp.passwd) == 0){
        strcpy(temp.passwd,_newpasswd.c_str());
    }else{
        throw "wrong passwd";
    }
    fstream file;file.open("userData.txt",fstream::binary | fstream::in | fstream::out);
    file.seekp(vec_ans[0]);
    file.write(r_cast(temp),sizeof(User));
    file.close();
    throw int(2);

}

bool isEmpty(string _isbn){
    vector<int> vec_ans;ISBN_ULL.FindOffset(_isbn,vec_ans);
    return vec_ans.empty();
}

pair<Book,int> getBook(string _isbn){
    vector<int> vec_ans;ISBN_ULL.FindOffset(_isbn,vec_ans);
    int position = vec_ans[0];
    fstream file;file.open("BookData.txt",fstream::binary | fstream::in | fstream::out);
    file.seekp(position);
    Book temp;file.read(r_cast(temp),sizeof(Book));
    file.close();
    return make_pair(temp,position);
}

void select(string _isbn){
    if(OnlineUser.empty()) throw "unlogged";
    if(getUser(OnlineUser.top().first).privilege < 3) throw "low privilige";
    if(isEmpty(_isbn)){
        Book temp(_isbn);
        fstream file;file.open("BookData.txt",fstream::binary | fstream::in | fstream::out | fstream::ate);
        file.seekp(0,ios::end);
        int position = file.tellp();file.write(r_cast(temp),sizeof(Book));file.close();
        OnlineUser.top().second = position;
        ISBN_ULL.addElement(ULL::UnrolledLinkedList<20>::Element(_isbn,position));
    }else{
        OnlineUser.top().second = getBook(_isbn).second;
    }
    throw int(2);
}

void importBook(int _quantity,double _cost){
    if(OnlineUser.empty()) throw "unlogged";
    if(getUser(OnlineUser.top().first).privilege < 3) throw "low privilige";
    int position = OnlineUser.top().second;
    if(position == -1) throw "unselected";
    fstream file;file.open("BookData.txt",fstream::binary | fstream::in | fstream::out);
    file.seekp(position);
    Book temp;file.read(r_cast(temp),sizeof(Book));
    temp.quantity += _quantity;
    file.seekp(position);file.write(r_cast(temp),sizeof(Book));file.close();
    manageMoney(_cost,false);
    throw int(2);

}

void buyBook(string _isbn,int _quantity){
    if(OnlineUser.empty()) throw "unlogged";
    if(getUser(OnlineUser.top().first).privilege < 1) throw "low privilige";
    if(isEmpty(_isbn)) throw "blank book";
    Book temp = getBook(_isbn).first;int position = getBook(_isbn).second;
    fstream file;file.open("BookData.txt",fstream::binary | fstream::in | fstream::out);
    file.seekp(position);
    if(temp.quantity < _quantity) throw "much more books";
    temp.quantity -= _quantity;
    file.seekp(position);file.write(r_cast(temp),sizeof(Book));file.close();
    printf("%0.2f",_quantity * temp.price);
    manageMoney(_quantity * temp.price,true);
    cout << endl;
    throw int(2);
}

void modifyBook(string mode,string token){
    if(OnlineUser.empty()) throw "unlogged";
    if(getUser(OnlineUser.top().first).privilege < 3) throw "low privilige";
    int position = OnlineUser.top().second;
    if(position == -1) throw "unselected";
    fstream file;file.open("BookData.txt",fstream::binary | fstream::in | fstream::out);
    file.seekp(position);
    Book temp;file.read(r_cast(temp),sizeof(Book));
    if(strcmp(mode.c_str(),"-ISBN") == 0){
        if(!isEmpty(token)) throw "invalid";
        ISBN_ULL.DeleteElement(ULL::UnrolledLinkedList<20>::Element(temp.ISBN,position));
        strcpy(temp.ISBN,token.c_str());
        ISBN_ULL.addElement(ULL::UnrolledLinkedList<20>::Element(token,position));
    }
    if(strcmp(mode.c_str(),"-name") == 0){
        if(temp.name[0] != 0) name_ULL.DeleteElement(ULL::UnrolledLinkedList<60>::Element(temp.name,position));
        strcpy(temp.name,token.c_str());
        name_ULL.addElement(ULL::UnrolledLinkedList<60>::Element(token,position));
    }
    if(strcmp(mode.c_str(),"-author") == 0){
        if(temp.author[0] != 0) author_ULL.DeleteElement(ULL::UnrolledLinkedList<60>::Element(temp.author,position));
        strcpy(temp.author,token.c_str());
        author_ULL.addElement(ULL::UnrolledLinkedList<60>::Element(token,position));
    }
    if(strcmp(mode.c_str(),"-price") == 0){
        stringstream ss(token);
        ss >> temp.price;
    }
    if(strcmp(mode.c_str(),"-keyword") == 0){
        if(strcmp(token.c_str(),"keyword1|1|1") == 0) throw "invalid";
        stringstream ss1(temp.keyword);
        int count2 = temp.keyword[0] == '\0' ? 0:1;char indkeyword1[60];
        for(int i = 0;i < 60 && temp.keyword[i] != '\0';++i) {
            if(temp.keyword[i] == '|') count2 ++;
        }
        while(count2--){
            ss1.getline(indkeyword1,60,'|');
            keyword_ULL.DeleteElement(ULL::UnrolledLinkedList<60>::Element(indkeyword1,position));
        }
        strcpy(temp.keyword,token.c_str());
        stringstream ss(token);
        int count = 1;char indkeyword[60];
        for(int i = 0;i < 60 && token[i] != '\0';++i) {
            if(token[i] == '|') count ++;
        }
        while(count--){
            ss.getline(indkeyword,60,'|');
            keyword_ULL.addElement(ULL::UnrolledLinkedList<60>::Element(indkeyword,position));
        }
    }
    file.seekp(position);file.write(r_cast(temp),sizeof(Book));file.close();
}

void showBook(int mode,string token){
    if(OnlineUser.empty()) throw "unlogged";
    if(getUser(OnlineUser.top().first).privilege < 1) throw "low privilige";
    vector<int> vec_ans;
    map<string,Book> map_ans;
    Book temp;
    fstream file;file.open("BookData.txt",fstream::binary | fstream::in | fstream::out);
    switch (mode) {
        case 1:
            ISBN_ULL.PrintULL(vec_ans);
            if(vec_ans.empty()){cout << endl; return;}
            for(auto it = vec_ans.begin();it != vec_ans.end();it++) {
                file.seekp(*it);file.read(r_cast(temp), sizeof(Book));
                cout << temp << endl;
            }
            break;
        case 2:
            ISBN_ULL.FindOffset(token,vec_ans);
            if(vec_ans.empty()){cout << endl; return;}
            file.seekp(vec_ans[0]);file.read(r_cast(temp),sizeof(Book));
            cout << temp << endl;
            break;
        case 3:
            name_ULL.FindOffset(token,vec_ans);
            if(vec_ans.empty()){cout << endl; return;}
            for(auto it = vec_ans.begin();it != vec_ans.end();it++) {
                file.seekp(*it);
                file.read(r_cast(temp), sizeof(Book));
                map_ans.insert(make_pair(temp.ISBN,temp));
            }
            for(auto it = map_ans.begin();it != map_ans.end();it++) {
                cout << (*it).second << endl;
            }
            break;
        case 4:
            author_ULL.FindOffset(token,vec_ans);
            if(vec_ans.empty()){cout << endl; return;}
            for(auto it = vec_ans.begin();it != vec_ans.end();it++) {
                file.seekp(*it);
                file.read(r_cast(temp), sizeof(Book));
                map_ans.insert(make_pair(temp.ISBN,temp));
            }
            for(auto it = map_ans.begin();it != map_ans.end();it++) {
                cout << (*it).second << endl;
            }
            break;
        case 5:
            keyword_ULL.FindOffset(token,vec_ans);
            if(vec_ans.empty()){cout << endl; return;}
            for(auto it = vec_ans.begin();it != vec_ans.end();it++) {
                file.seekp(*it);
                file.read(r_cast(temp), sizeof(Book));
                map_ans.insert(make_pair(temp.ISBN,temp));
            }
            for(auto it = map_ans.begin();it != map_ans.end();it++) {
                cout << (*it).second << endl;
            }
            break;
    }
    file.close();
    throw int(2);
}

void showFinance(int times){
    if(OnlineUser.empty()) throw "unlogged";
    if(getUser(OnlineUser.top().first).privilege < 7) throw "low privilige";
    double benefit = 0,cost = 0;int counter;pair<double,double> temppair;double tempdata;
    fstream file;file.open("FinanceData.txt",fstream::binary | fstream::in | fstream::out);
    file.read(r_cast(counter),sizeof(int));file.read(r_cast(temppair),sizeof(pair<double,double>));
    if(times > counter) throw "invalid";
    if(times == 0){
        cout << "+ ";
        printf("%0.2f",temppair.first);
        cout << " - ";
        printf("%0.2f",temppair.second);
        cout << endl;
    }else{
        while(times){
            file.seekp(sizeof(int) + sizeof(pair<double,double>) + sizeof(double) * (counter - times),ios::beg);
            file.read(r_cast(tempdata),sizeof(double));
            if(tempdata > 0) benefit += tempdata;
            if(tempdata < 0) cost -= tempdata;
            times--;
        }
        cout << "+ ";
        printf("%0.2f",benefit);
        cout << " - ";
        printf("%0.2f",cost);
        cout << endl;
    }
    file.close();
    throw int(2);
}

void report(int mode){
    if(mode == 1){
        fstream file;file.open("logData.txt",ios::out | ios::in | ios::binary);
        vector<int> vec_ans;log_ULL.PrintULL(vec_ans);
        char tempToken[60];
        cout << "--------------Output all operations log." << endl;
        for(int i = 0;i < vec_ans.size();++i){
            file.seekp(vec_ans[i]);
            file.read(r_cast(tempToken),sizeof(tempToken));
            cout << tempToken << endl;
        }
        cout << "--------------Output all operations log finished." << endl;
        file.close();
        return;
    }
    if(mode == 2){//report myself
        fstream file;file.open("logData.txt",ios::out | ios::in | ios::binary);
        vector<int> vec_ans;log_ULL.FindOffset(OnlineUser.top().first,vec_ans);
        char tempToken[60];
        cout << "--------------Output my operations log." << endl;
        for(int i = 0;i < vec_ans.size();++i){
            file.seekp(vec_ans[i]);
            file.read(r_cast(tempToken),sizeof(tempToken));
            cout << tempToken << endl;
        }
        cout << "--------------Output my operations log finished." << endl;
        file.close();
        return;
    }
}