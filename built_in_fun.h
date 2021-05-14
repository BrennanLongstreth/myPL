#ifndef BUILT_IN_FUN_H
#define BUILT_IN_FUN_H
//File to add built in function defintions to the C++ code
#include <iostream>

using namespace std;

void print(string s) {
    cout << s;
}

string itos(int s) {
    return to_string(s);
}

// int stoi(string s) {
//    return stoi(s);
// }

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
    //cout << s.substr(i,1) << "-------" << endl;
    return s.substr(i,1);
}

string read() {
    string s;
    getline(cin,s);
    return s;
}

#endif