#include"tm.h"
#include<iostream>
#include<string>
using namespace std;

bool help_mode = false;
bool verbose_mode = false;
 
int main(int argc,char *argv[])
{
    verbose_mode = false;
    if(argc<3){
        PrintError("Too Few arguments");
    }
    string s;
    string file_name, input;
    for(int i=0;i<argc;i++){
        s = string(argv[i]);
        if(s == string("--help") || s == string("-h"))help_mode = true;
        if(s == string("--verbose") || s== string("-v"))verbose_mode = true;
    }
    if(help_mode){
        cout<<"usage: turing [-v|--verbose] [-h|--help] <tm> <input>"<<endl;
        return 0;
    }
    int idx = 0;
    for(int i = 1; i < argc; i++){
        if(argv[i][0] == '-')continue;
        if(idx == 0) file_name = string(argv[i]);
        else input = string(argv[i]);
        idx++;
    }
    //cout<<file_name<<endl;
    //cout<<input<<endl;

    TM *tm = new TM(file_name);
    //tm->PrintTM();
    tm->RunTM(input);
    tm->PrintResult();
    return 0;
}