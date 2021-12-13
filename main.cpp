#include"tm.h"
#include<iostream>
using namespace std;

 
int main()
{
    TM *tm = new TM("palindrome_detector_2tapes.tm");
    tm->PrintTM();
    string s;
    cin>>s;
    tm->RunTM(s);
    return 0;
}