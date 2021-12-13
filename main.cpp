#include"tm.h"
#include<iostream>
using namespace std;

 
int main()
{
    string s;
    getline(cin, s);
    //Delta *d = new Delta(s);
    for(char c: s){
        cout<<c<<endl;
    }
    
    return 0;
}