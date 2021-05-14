// --------------------------------------------------------
// Name: Brennan Longstreth
// File: built_in_fun.h
// Contributors: 
// Descritpion: File to define built in functions (for the C++
//              compiler after translation) 
// Notes: 
// Date Edited: 5/5/21
// --------------------------------------------------------

#ifndef BUILT_IN_FUN_H
#define BUILT_IN_FUN_H

#include <iostream>

using namespace std;

void print(string s) {
    cout << s;
}

string itos(int s) {
    return to_string(s);
}

// already built in to c++
// int stoi(string s) {
//    return stoi(s);
// }

// already built in to c++
//void stod() {
//
//}

string dtos(double x) {
    return to_string(x);
}

int length(string s) {
    return s.length();
}

string get(int i, string s) {
    return s.substr(i,1);
}

string read() {
    string s;
    getline(cin,s);
    return s;
}

#endif
