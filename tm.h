#pragma once
#include<string>
#include<vector>
#include<iostream>
using namespace std;

//前置声明
class Delta;
class State;
class TM;


void PrintError(string s);


//转换函数
class Delta{
private:
    TM *tm;//所在图灵机
    string old_state;           //旧状态
    string new_state;           //新状态
    string old_symbols;         //旧符号：因为可能是多带，所以多个带的负号用vector来装
    string new_symbols;         //新符号
    string dir;                 //读头移动方向
public:
    //构造函数，通过一行标准式子来判断
    Delta(const string& s, TM *tm);
    //给定旧状态和旧符号，判断是否匹配
    bool Match(string s_name, string syms){
        if(s_name!=old_state)return false; 
        for(int i=0;i<old_symbols.size();i++){
            if(old_symbols[i]=='*')continue;
            if(old_symbols[i] != syms[i])return false;
        }
        return true;
    }
    int NumofStar(){
        int n=0;
        for(int i=0;i<old_symbols.size();i++){
            if(old_symbols[i]=='*')n++;
        }
        return n;
    }

    //获得新状态，和新符号新方向
    string GetNewState(){return new_state;}
    string GetNewSymbols(){return new_symbols;}
    string GetDirect(){return dir;}

    //方便输出
    friend ostream& operator<<(ostream& out,const Delta& d){
        out<<d.old_state<<" "<<d.new_state<<" "<<d.old_symbols<<" "<<d.new_symbols<<" "<<d.dir;
        return out;
    }
};

//纸带
class Tape{
private://实际上我用两个string来模拟两段无限长的纸带
    string tapepos;//从input开头到最右端
    string tapeneg;//从input左边到最左端
public:
    int ptr;//当前的head
    Tape(){ptr = 0;tapepos +='_';}//空白纸带
    Tape(const string& s){ptr = 0; tapepos = s;}//第一条纸带，装input的那个
    
    char GetCurSym()const{//获得当前符号
        if(ptr>=0)
            return tapepos[ptr];
        else
            return tapeneg[-ptr-1];
    }
    void Move(char newsym, char direct){//根据新的符号和direct移动
        SetNewSym(newsym);
        MoveForward(direct);
    }
    void Print(){
        bool start = false;
        int p = tapeneg.size()-1;
        for(p=tapeneg.size()-1;p>=0;p--){
            if(tapeneg[p]!='_')cout<<tapeneg[p];
        }
        p = tapepos.size()-1;
        for(p = 0;p<tapepos.size();p++){
            if(tapepos[p]!='_')cout<<tapepos[p];
        }
        cout<<endl;
    }
    void PrintID(int idx){
        int start = -1000;
        cout<<"Index"<<idx<<" : ";
        int p;
        for(p=tapeneg.size()-1;p>=0;p--){
            if(start==-1000 && tapeneg[p]!='_')start = -p-1;
            if(tapeneg[p]!='_')cout<<p<< " ";
        }

        for(p = 0;p<tapepos.size();p++){
            if(start==-1000 && tapepos[p]!='_')start = p;
            if(tapepos[p]!='_')cout<<p<< " ";
        }
        if(start==-1000)cout<<abs(ptr);
        cout<<endl;

        cout<<"Tape"<<idx<<"  : ";
        for(p=tapeneg.size()-1;p>=0;p--){
            if(start==-1000 && tapeneg[p]!='_')start = -p-1;
            if(tapeneg[p]!='_')cout<<tapeneg[p]<< " ";
        }
        for(p = 0;p<tapepos.size();p++){
            if(start==-1 && tapepos[p]!='_')start = p;
            if(tapepos[p]!='_')cout<<tapepos[p]<< " ";
        }
        if(start == -1000)cout<<"_";
        cout<<endl;
        
        //输出head
        cout<<"Head"<<idx<<"  : ";
        if(start!=-1000)for(int i=0;i<ptr-start;i++)cout<<" ";
        cout<<"^";
        cout<<endl;
    }

private:
//下面辅助函数，设置新符号and移动读头
    void SetNewSym(char newsym){
        if(newsym == '*')return;
        if(ptr>=0) tapepos[ptr] = newsym;
        else tapeneg[-ptr-1] = newsym;
    }
    void MoveForward(char direct){
        if(direct=='l')ptr--;
        else if(direct =='r')ptr++;
        else return;
        //下面扩充纸带
        if(ptr>=0){ 
            if(tapepos.size()<ptr+1)tapepos +='_';
        }else{
            if(tapeneg.size()<-ptr)tapeneg +='_';
        }
    }
};


class TM{
private:
//七元组
    vector<string> state_set;           //状态集合
    string input_symbol_set;            //输入符号集合
    string tape_symbol_set;             //纸带符号集合 = 输入符号集合+空白符号
    vector<Delta *> deltas;             //转移函数
    string init_state;                  //初始状态
    char blank_symbol;                  //空白符号
    vector<string> final_states;        //接受状态
//运行时
    int num_of_tapes;                   //纸带的数目
    vector<Tape *> tapes;               //纸带
    int step = 0;                       //第几步
    string cur_state;                   //当前状态
public:
    TM(const string& tm_file);
    int GetTapeNum() const { return num_of_tapes; }
    //处理输入的每一行
    void ParseString(const string& s);

    string SearchState(const string& name) const{           //搜索图灵机是否存在这个状态
        for(string state: state_set)
            if(state == name)return state;
        return string();
    }
    bool SearchInputSymbol(char sym){                       //搜索图灵机是否存在这个input符号，检查input时候用
        if(sym == '*')return true;
        for(int i= 0; i < input_symbol_set.size(); i++)
            if(input_symbol_set[i] == sym) return true;
        return false;
    }  
    bool SearchSymbol(char sym){                            //搜索图灵机是否存在这个符号
        char c;
        if(sym == '*')return true;
        for(int i= 0; i < tape_symbol_set.size(); i++){
            c = tape_symbol_set[i];
            if(c == sym)
                return true;
        }
        return false;
    }  
    void AddState(const string& name){ this->state_set.push_back(name); }
    void AddFinalState(const string& name){ this->final_states.push_back(name); }
    void AddDelta(Delta *delta){ deltas.push_back(delta); }

    void CheckInput(string input){
        for(int i=0;i<input.size();i++){
            if(!this->SearchInputSymbol(input[i])){
                string s= string("error: \'") + input[i] + "\' was not declared in the set of input symbols\nInput: "+input+"\n"+"       ";
                for(int j=0;j<i;j++)s+=' ';
                s+='^';
                PrintError(s);
            }
        }
    }
    //初始化，和根据input运行图灵机
    void InitTapes(string input);
    void RunTM(string input);
    
    string GetCurSymbols(){
        string s;
        char c;
        for(Tape *tp : tapes){
            s += tp->GetCurSym();
        }
        return s;
    }

    Delta *GetDelta(string state_name, string syms){
        int numofstar = 10000;
        Delta *res = nullptr;
        for(Delta *dt : deltas){
            if(dt->Match(state_name, syms)){
                if(numofstar > dt->NumofStar()){
                    res = dt;
                    numofstar = dt->NumofStar();
                }
            }
        }
        return res;
    }
    void PrintTM();
    void PrintResult(){
        tapes[0]->Print();
    }
    void PrintCur(){
        cout<<"Step : "<<step<<endl;
        for(int i=0;i<num_of_tapes;i++){
            tapes[i]->PrintID(i);
        }
        cout<<"---------------------------------------------"<<endl;
    }

};

