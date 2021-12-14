#include"tm.h"
#include<string.h>
#include<iostream>
#include<fstream>
#include<unistd.h>

Delta::Delta(const string& s, TM *tm){
    this->tm = tm;
    //tm->PrintTM();
    vector<string> substrs;
    const char *delim = " ";
    char *sptr;
    sptr = strtok(const_cast<char *>(s.c_str()), delim);
    while(sptr){
        string substr = string(sptr);
        //cout<<substr<<endl;
        substrs.push_back(substr);//把空格分出的子串传入substrs
        sptr = strtok(NULL, delim);
    }
    
    //开始解析
    //1.首先解析状态
    old_state = tm->SearchState(substrs[0]);
    if(old_state == string()){PrintError("syntax error",false);}//错误，不存在这个状态

    if(substrs[1].size() != substrs[2].size()){PrintError("syntax error",false);}

    //2.对旧的符号进行解析
    for(int i = 0; i < substrs[1].size(); i++){
        if(!tm->SearchSymbol(substrs[1][i]))//:错误，不存在这个符号
            PrintError("syntax error",false);
    }
    old_symbols = substrs[1];
    
    //3.对新的符号进行解析
    for(int i = 0; i < substrs[2].size(); i++){
        if(!tm->SearchSymbol(substrs[2][i])){
            //TODO:错误，不存在这个符号
            PrintError("syntax error",false);
        }
    }
    new_symbols = substrs[2];

    //4.方向
    for(int i = 0; i < substrs[3].size();i++){
        if(substrs[3][i] !='l' && substrs[3][i] !='r' && substrs[3][i] != '*'){
            PrintError("syntax error",false);
        }
    }
    dir = substrs[3];
    
    //5.下一个状态
    new_state = tm->SearchState(substrs[4]);
    if(new_state == string()) PrintError("syntax error",false);//错误，不存在这个状态
    tm->AddDelta(this);
}


TM::TM(const string& tm_file){
    verbose_mode = false;
    ifstream file(tm_file.c_str());
    string s;
    while(getline(file, s)){
        if(s.size() == 0 || s[0] == ';') continue;//遇到空行或者注释
        ParseString(s);
    }
    file.close();
    step = 0;
}

void TM::ParseString(const string& s){
    if(s.size()<=3) return;
    //cout<<"111"<<endl;

    Delta *delta;
    if(s[0]!='#')//不是井号开头，说明是delta
        delta = new Delta(s, this);

    string substr;
    vector<string> substrs;
    int start = -1, len;
    for(int i=0;i<s.length();i++){
        if(s[i]== '{'){
            start = i+1;
            len = -2;
        }
        len++;
        if(s[i]=='}')break;
    }
    if(start > 0)substr = s.substr(start, len);
    
    const char *delim;
    char *sptr;
    int num;
    string strofnum;
    switch(s[1]){
    case 'Q':
        delim = ",";
        sptr = strtok(const_cast<char *>(substr.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            this->AddState(ss);
            sptr = strtok(NULL, delim);
        }
        break;
    case 'S':
        delim = ",";
        sptr = strtok(const_cast<char *>(substr.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            input_symbol_set += ss[0];
            sptr = strtok(NULL, delim);
        }
        break;
    case 'G':
        delim = ",";
        sptr = strtok(const_cast<char *>(substr.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            tape_symbol_set += ss[0];
            sptr = strtok(NULL, delim);
        }
        break;
    case 'q':
        delim = " ";
        sptr = strtok(const_cast<char *>(s.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            substrs.push_back(ss);//把空格分出的子串传入substrs
            sptr = strtok(NULL, delim);
        }
        init_state = substrs[substrs.size()-1];
        break;
    case 'B':
        delim = " ";
        sptr = strtok(const_cast<char *>(s.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            substrs.push_back(ss);//把空格分出的子串传入substrs
            sptr = strtok(NULL, delim);
        }
        blank_symbol = substrs[substrs.size()-1][0];
        break;
    case 'F':
        delim = ",";
        sptr = strtok(const_cast<char *>(substr.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            this->AddFinalState(ss);
            sptr = strtok(NULL, delim);
        }
        break;
    case 'N':
        delim = " ";
        sptr = strtok(const_cast<char *>(s.c_str()), delim);
        while(sptr){
            string ss = string(sptr);
            substrs.push_back(ss);//把空格分出的子串传入substrs
            sptr = strtok(NULL, delim);
        }
        num = 0;
        strofnum = substrs[substrs.size()-1];
        for(int i = 0; i < strofnum.size(); i++){
            num = num * 10 + (strofnum[i] - '0');
        }
        num_of_tapes = num;
        break;
    default: 
        break;
    }
    
}


void TM::RunTM(string input){
    if(verbose_mode)cout<<"Input: "<<input<<endl;
    CheckInput(input);
    if(verbose_mode)cout<<"==================== RUN ===================="<<endl;
    InitTapes(input);

    while(1){
        //usleep(1000000);
        PrintCur();
        step++;
        string syms = GetCurSymbols();
        //cout<<"CurState before: "<<syms<<endl;
        Delta *dt = GetDelta(cur_state, syms);
        if(!dt) break;
        //cout<<"CurState: "<<cur_state<<endl;
        cur_state = dt->GetNewState();
        
        string newsyms = dt->GetNewSymbols();
        //cout<<"NewSyms: "<<newsyms<<endl;
        syms = GetCurSymbols();
        //cout<<"CurSyms after modified: "<<syms<<endl;
        //cout<<"CurState after modified: "<<cur_state<<endl;
        string direct = dt->GetDirect();
        for(int i=0;i<num_of_tapes;i++){//对于每一条磁带
           tapes[i]->Move(newsyms[i], direct[i]);
        }

        
    }

}

//初始化N条纸带，然后把第一条放上input，后面初始化为空白，依次push进tapes中。
void TM::InitTapes(string input){
    cur_state = init_state;
    Tape *first_tape = new Tape(input);
    tapes.push_back(first_tape);
    for(int i=0;i<num_of_tapes-1;i++){
        Tape *tape = new Tape();
        tapes.push_back(tape);
    }

}


void TM::PrintTM(){
    cout<<"Q:"<<endl;
    for(string st : state_set){
        cout<<st<<" ";
    }
    cout<<endl;

    cout<<"Input Symbol Set: "<<input_symbol_set<<endl;
    cout<<"Tape Symbol Set: "<<tape_symbol_set<<endl;
    cout<<"Init State: "<<init_state<<endl;
    cout<<"Blank State: "<<blank_symbol<<endl;
    cout<<"Num of Tapes: "<<num_of_tapes<<endl;

    cout<<"Delta:"<<endl;
    for(Delta *d : deltas){
        cout<<(*d)<<endl;
    }

}


void PrintError(string s,bool verbose){

    if(verbose)cerr<<"==================== ERR ===================="<<endl;
    cerr<<s<<endl;
    if(verbose)cerr<<"==================== END ===================="<<endl;
    exit(-1);
}


