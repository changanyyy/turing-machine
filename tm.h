#pragma once
#include<string>
#include<vector>
#include<iostream>
using namespace std;

class Delta;
class State;
class TM;

class State{
private:
    string name;
    vector<Delta *> deltas;
public:
    State(const string& name){this->name = name;}
    void AddDelta(Delta *delta){this->deltas.push_back(delta);}
    bool IsState(const string& s) const{return name == s;}
    string GetName()const{return name;}
};


class Delta{
private:
    TM *tm;//所在图灵机
    State *old_state;                   //旧状态
    State *new_state;                   //新状态
    string old_symbols;         //旧符号：因为可能是多带，所以多个带的负号用vector来装
    string new_symbols;         //新符号
    string dir;   //读头移动方向
public:
    Delta(const string& s, TM *tm);

    friend ostream& operator<<(ostream& out,const Delta& d){
        out<<d.old_state->GetName()<<" ";
        out<<d.new_state->GetName()<<" ";
        out<<d.old_symbols<<" ";
        out<<d.new_symbols<<" ";
        out<<d.dir;
        return out;
    }
};


class TM{
private:
    vector<State *> state_set;          //状态集合
    string input_symbol_set;            //输入符号集合
    string tape_symbol_set;             //纸带符号集合 = 输入符号集合+空白符号
    vector<Delta *> deltas;               //转移函数
    string init_state;                  //初始状态
    char blank_symbol;                  //空白符号
    vector<string> final_states;                //接受状态

    int num_of_tape;
public:
    TM(const string& tm_file);
    int GetTapeNum() const{ return num_of_tape;}

    void ParseString(const string& s);

    State *SearchState(const string& name) const{      //搜索图灵机是否存在这个状态
        for(State *state: state_set)
            if(state->IsState(name)) 
                return state;
        return nullptr;
    }
    bool SearchInputSymbol(char sym){                       //搜索图灵机是否存在这个符号
        char c;
        if(sym == '*')return true;
        for(int i= 0; i < input_symbol_set.size(); i++){
            c = input_symbol_set[i];
            if(c == sym)
                return true;
        }
        return false;
    }  
    bool SearchSymbol(char sym){                       //搜索图灵机是否存在这个符号
        char c;
        if(sym == '*')return true;
        for(int i= 0; i < tape_symbol_set.size(); i++){
            c = tape_symbol_set[i];
            if(c == sym)
                return true;
        }
        return false;
    }  
    void AddState(const string& name){
        State *newstate = new State(name);
        this->state_set.push_back(newstate);
    }
    void AddFinalState(const string& name){this->final_states.push_back(name);}
    void AddDelta(Delta *delta){ deltas.push_back(delta);}

    bool CheckInput(string input){
    for(int i=0;i<input.size();i++){
        if(!this->SearchInputSymbol(input[i])){
            string s= string("error: \'") + input[i] + "\' was not declared in the set of input symbols\nInput: "+input+"\n"+"       ";
            for(int j=0;j<i;j++)s+=' ';
            s+='^';
            PrintError(s);
        }
    }
}
    void RunTM(string input);

    void PrintTM();
};

void PrintError(string s);