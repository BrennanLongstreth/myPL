// AUTO GENERATED CODE: 
// Translator written by Brennan Longstreth

#include <iostream>
#include "built_in_fun.h"

using namespace std;

int main() {
   print("String Tests: \n");
   print((string("  Should be 'a': ") + (string("a") + "\n")));
   print((string("  Should be 'a': ") + ('a' + "\n")));
   print((string("  Should be 'a': ") + (string((string("") + 'a')) + "\n")));
   print("  Should be true 'abc' < 'abd': ");
   if ("abc" < "abd") {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 'abc' <= 'abd': ");
   if ("abc" <= "abd") {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 'abd' > 'abc': ");
   if ("abd" > "abc") {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 'abc' >= 'abc': ");
   if ("abc" >= "abc") {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 'abc == 'abc': ");
   if ("abc" == "abc") {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 'abd != 'abc': ");
   if ("abd" != "abc") {
      print("true\n");
   } else {
      print("false\n");
   };
   print("Integer Tests: \n");
   print((string("  Should be '5': ") + (itos((2 + 3)) + "\n")));
   print((string("  Should be '9': ") + (itos((2 + (3 + 4))) + "\n")));
   print((string("  Should be '6': ") + (itos((2 * 3)) + "\n")));
   print((string("  Should be '6': ") + (itos((1 * (2 * 3))) + "\n")));
   print((string("  Should be '1': ") + (itos((3 / 2)) + "\n")));
   print((string("  Should be '2': ") + (itos((6 / 3)) + "\n")));
   print((string("  Should be '0': ") + (itos((6 % 3)) + "\n")));
   print((string("  Should be '-1': ") + (itos((!1)) + "\n")));
   print("  Should be true 3 < 4: ");
   if (3 < 4) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 3 <= 4: ");
   if (3 <= 4) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 4 > 3: ");
   if (4 > 3) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 4 >= 3: ");
   if (4 >= 3) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 4 == 4: ");
   if (4 == 4) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 4 != 3: ");
   if (4 != 3) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true not 4 != 4: ");
   if (!(4 != 4)) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("Double Tests: \n");
   print((string("  Should be '5.3': ") + (dtos((2.1 + 3.2)) + "\n")));
   print((string("  Should be '9.6': ") + (dtos((2.1 + (3.2 + 4.3))) + "\n")));
   print((string("  Should be '6.6': ") + (dtos((2.2 * 3.0)) + "\n")));
   print((string("  Should be '8.866': ") + (dtos((1.3 * (2.2 * 3.1))) + "\n")));
   print((string("  Should be '1.75': ") + (dtos((3.5 / 2.0)) + "\n")));
   print((string("  Should be '2.2': ") + (dtos((6.6 / 3.0)) + "\n")));
   print((string("  Should be '-3.4': ") + (dtos((!3.4)) + "\n")));
   print("  Should be true 3.1 < 4.2: ");
   if (3.1 < 4.2) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 3.1 <= 4.2: ");
   if (3.1 <= 4.2) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 4.2 > 3.1: ");
   if (4.2 > 3.1) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 4.2 >= 3.1: ");
   if (4.2 >= 3.1) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 4.2 == 4.2: ");
   if (4.2 == 4.2) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 4.2 != 3.1: ");
   if (4.2 != 3.1) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("Bool Tests: \n");
   print("  Should be true (not false): ");
   if (!(false)) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true (true and true): ");
   if (true && (true)) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true (not false and true): ");
   if (!(false && (true))) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true ((not false) and true): ");
   if ((!(false)) && (true)) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true (not (true and false)): ");
   if (!(true && (false))) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true (true or false): ");
   if (true || (false)) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true (false or true): ");
   if (false || (true)) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true (false or (not false)): ");
   if (false || ((!(false)))) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true (not false or false): ");
   if (!(false || (false))) {
      print("true\n");
   } else {
      print("false\n");
   };
   print("Char Tests: \n");
   print("  Should be true 'a' < 'b': ");
   if ('a' < 'b') {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 'a' <= 'a': ");
   if ('a' <= 'a') {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 'd' > 'c': ");
   if ('d' > 'c') {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 'b' >= 'a': ");
   if ('b' >= 'a') {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 'a' == 'a': ");
   if ('a' == 'a') {
      print("true\n");
   } else {
      print("false\n");
   };
   print("  Should be true 'b' != 'a': ");
   if ('b' != 'a') {
      print("true\n");
   } else {
      print("false\n");
   };
}

