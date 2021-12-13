#include"tm.h"
#include<string.h>
#include<iostream>
#include<fstream>


Delta::Delta(const string& s, TM *tm){
    this->tm = tm;

    vector<string> substrs;
    const char *delim = " ";
    char *sptr;
    sptr = strtok(const_cast<char *>(s.c_str()), delim);
    while(sptr){
        string substr = string(sptr);
        substrs.push_back(substr);//把空格分出的子串传入substrs
        sptr = strtok(NULL, delim);
    }
    
    //开始解析
    
    //1.首先解析状态
    old_state = tm->SearchState(substrs[0]);
    if(old_state == nullptr){
        //TODO:错误，不存在这个状态
        PrintError("Delta: Old state error.");
    }

    if(substrs[1].size() != tm->GetTapeNum() 
        || substrs[2].size() != tm->GetTapeNum()
            || substrs[3].size() != tm->GetTapeNum() ){
        PrintError("Delta: symbol num error.");
    }

    //2.对旧的符号进行解析
    for(int i = 0; i < substrs[1].size(); i++){
        if(!tm->SearchSymbol(substrs[1][i])){
            //TODO:错误，不存在这个符号
            PrintError("Delta: Old symbol doesn't exist.");
        }
    }
    old_symbols = substrs[1];
    
    //3.对新的符号进行解析
    for(int i = 0; i < substrs[2].size(); i++){
        if(!tm->SearchSymbol(substrs[2][i])){
            //TODO:错误，不存在这个符号
            PrintError("Delta: New symbol doesn't exist.");
        }
    }
    new_symbols = substrs[2];

    //4.方向
    for(int i = 0; i < substrs[3].size();i++){
        if(substrs[3][i] !='l' && substrs[3][i] !='r'){
            PrintError("Delta: direction error.");
        }
    }
    dir = substrs[3];
    
    //5.下一个状态
    new_state = tm->SearchState(substrs[4]);
    if(new_state == nullptr){
        //TODO:错误，不存在这个状态
        PrintError("Delta: New state error.");
    }

    tm->AddDelta(this);
}




TM::TM(const string& tm_file){
    ifstream file(tm_file.c_str());
    string s;
    while(getline(file, s)){
        if(s.size() == 0 || s[0] == ';') continue;//遇到空行或者注释
        ParseString(s);
    }



    file.close();
}

void TM::ParseString(const string& s){
    if(s.size()<=3) return;
    
    Delta *delta;
    if(s[0]!='#')//不是井号开头，说明是delta
        delta = new Delta(s, this);

    string substr;
    vector<string> substrs;
    int start = -1, len;
    for(int i=0;i<s.length();i++){
        if(s[i]== '{'){
            start = i;
            len = 0;
        }
        len++;
        if(s[i]=='}')break;
    }
    if(start > 0)substr = s.substr(start, len);
    
    switch(s[1]){
    case 'Q':
        const char *delim = ",";
        char *sptr;
        sptr = strtok(const_cast<char *>(substr.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            this->AddState(ss);
            sptr = strtok(NULL, delim);
        }
        break;
    case 'S':
        const char *delim = ",";
        char *sptr;
        sptr = strtok(const_cast<char *>(substr.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            input_symbol_set += ss[0];
            sptr = strtok(NULL, delim);
        }
        break;
    case 'G':
        const char *delim = ",";
        char *sptr;
        sptr = strtok(const_cast<char *>(substr.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            tape_symbol_set += ss[0];
            sptr = strtok(NULL, delim);
        }
        break;
    case 'q':
        const char *delim = " ";
        char *sptr;
        sptr = strtok(const_cast<char *>(s.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            substrs.push_back(ss);//把空格分出的子串传入substrs
            sptr = strtok(NULL, delim);
        }
        init_state = substrs[substrs.size()-1];
        break;
    case 'B':
        const char *delim = " ";
        char *sptr;
        sptr = strtok(const_cast<char *>(s.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            substrs.push_back(ss);//把空格分出的子串传入substrs
            sptr = strtok(NULL, delim);
        }
        blank_symbol = substrs[substrs.size()-1][0];
        break;
    case 'F':
        const char *delim = ",";
        char *sptr;
        sptr = strtok(const_cast<char *>(substr.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            this->AddFinalState(ss);
            sptr = strtok(NULL, delim);
        }
        break;
    case 'N':
        const char *delim = " ";
        char *sptr;
        sptr = strtok(const_cast<char *>(s.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            substrs.push_back(ss);//把空格分出的子串传入substrs
            sptr = strtok(NULL, delim);
        }
        int num = 0;
        string strofnum = substrs[substrs.size()-1];
        for(int i = 0; i < strofnum.size(); i++){
            num = num * 10 + strofnum[i];
        }
        num_of_tape = num;
        break;
    default: 
        break;
    }
    
}


void PrintError(string s){
    cerr<<s<<endl;
    exit(-1);
}


