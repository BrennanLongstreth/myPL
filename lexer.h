//----------------------------------------------------------------------
// NAME:Brennan Longstreth
// FILE:lexer.h
// DATE:2/15/2020
// DESC:decides on what token to apply to each word in  
//----------------------------------------------------------------------

#ifndef LEXER_H
#define LEXER_H

#include <istream>
#include <string>
#include "token.h"
#include "mypl_exception.h"


class Lexer
{
public:

  // construct a new lexer from the input stream
  Lexer(std::istream& input_stream);

  // return the next available token in the input stream (including
  // EOS if at the end of the stream)
  Token next_token();
  
private:

  // input stream, current line, and current column
  std::istream& input_stream;
  int line;
  int column;

  // return a single character from the input stream and advance
  char read();

  // return a single character from the input stream without advancing
  char peek();

  // create and throw a mypl_exception (exits the lexer)
  void error(const std::string& msg, int line, int column) const;

};


Lexer::Lexer(std::istream& input_stream)
  : input_stream(input_stream), line(1), column(0)
{
}


char Lexer::read()
{
  column++;
  return input_stream.get();
}


char Lexer::peek()
{
  return input_stream.peek();
}


void Lexer::error(const std::string& msg, int line, int column) const
{
  throw MyPLException(LEXER, msg, line, column);
}


Token Lexer::next_token()
{
  std::string lexeme = "";
  char ch = read();
  
  //1 Read through white space 
  while(isspace(ch) || ch == '#') {
    while(isspace(ch)) {
      if(ch == '\n') {
        column = 0;
        line++;
      }
      ch = read();
    }
    while(ch == '#') {
      while(ch != '\n') {
        ch = read();
      }
      column = 0;
      line++;
      ch = read();
    }
  }

  //2 check for eof
  if(ch == EOF) {
    return Token(EOS,"",line,column);
  }

  //set line and column for start of token
  int l = line;
  int c = column;

  //3 check for simple symbols
      //basic symbols
  if(ch == '(') {
    return Token(LPAREN,"(",l,c);
  }
  if(ch == ')') {
    return Token(RPAREN,")",l,c);
  }
  if(ch == ',') {
    return Token(COMMA,",",l,c);
  }
  if(ch == '.') {
    if(isdigit(peek())) {
      error("missing leading digit in double value",l,c);
    }
    return Token(DOT,".",l,c);
  }
  if(ch == ':') {
    return Token(COLON,":",l,c);
  }
    //math symbols
  if(ch == '+') {
    return Token(PLUS,"+",l,c);
  }
  if(ch == '-') {
    return Token(MINUS,"-",l,c);
  }
  if(ch == '*') {
    return Token(MULTIPLY,"*",l,c);
  }
  if(ch == '/') {
    return Token(DIVIDE,"/",l,c);
  }
  if(ch == '%') {
    return Token(MODULO,"%",l,c);
  }


  //4 check for involved symbols
  if(ch == '!') {
    if(peek() == '=') {
      read();
      return Token(NOT_EQUAL,"!=",l,c);
    }
    else {
      if(peek() == '!') {
        error("double negation not allowed",l,c);
      }
      return Token(NOT,"!",l,c);
    }
  }
  if(ch == '=') {
    if(peek() != '=') {
      return Token(ASSIGN,"=",l,c);
    }else{
      read();
      return Token(EQUAL,"==",l,c);
    }
  }
  if(ch == '<') {

    if(peek() == '=') {
      read();
      return Token(LESS_EQUAL,"<=",l,c);
    }else {
      return Token(LESS,"<",l,c);
    }
  }
  if(ch == '>') {
    if(peek() == '=') {
      read();
      return Token(GREATER_EQUAL,">=",l,c);
    }    
    else{
      return Token(GREATER,">",l,c);
    }
  }
  
  //5 check for char values
  if(ch == '\'') {
    std::string temp = "";
    temp += read();
    if(peek() == '\'') {
      read(); // removes end quote
    }else {
      error("Invalid ending character for char", l, c);
    }
    return Token(CHAR_VAL,temp,l,c);
  }

  //6 check for string values
  if(ch == '"') {
    std::string temp = "";
    while(peek() != '"') {
      if(peek() == '\n') {
        error("multiline strings are not allowed",l,c);
      }
      temp += read();
    }
    read(); // removes end quote
    return Token(STRING_VAL,temp,l,c);
  }

  //7 check for numeric values
  if(isdigit(ch)) {
    std::string temp = "";
    temp += ch;
    while(isdigit(peek())){
      ch = read();
      temp += ch;
    }
    if(temp.length() > 1 && temp[0] == '0') {
      error("leading 0 not allowed in this case",l,c);
    }
    if(peek() != '.') { // pulls out the integer case
      return Token(INT_VAL,temp,l,c);
    }
    
    ch = read();
    temp += ch;
    if(!isdigit(peek())) {
      error("Needs at least one digit after '.' ",l,c);
    }
    while(isdigit(peek())){
      ch = read();
      temp += ch;
    }
    return Token(DOUBLE_VAL,temp,l,c);
  }

  //8 check for reserved words
  int KEYWORDS = 26;
  std::string lex [KEYWORDS]= {"and", "bool", "char", "do", "double", "end", "elseif", "else", "fun", "for", "false", "if", "int", "neg", "new", "nil", "not", "or", "return", "string", "type", "to", "then", "true", "var", "while"};
  TokenType tokenLookup [KEYWORDS] = {AND, BOOL_TYPE, CHAR_TYPE, DO, DOUBLE_TYPE, END, ELSEIF, ELSE, FUN, FOR, BOOL_VAL, IF, INT_TYPE, NEG, NEW, NIL, NOT, OR, RETURN, STRING_TYPE, TYPE, TO, THEN, BOOL_VAL, VAR, WHILE};
  std::string temp = "";
  temp += ch;
  while(isalpha(peek()) || isdigit(peek()) || peek() == '_') {
    ch = read();
    temp += ch;
  }

  for(int i = 0; i < KEYWORDS; i++) {
    if(!temp.compare(lex[i])) {
      return Token(tokenLookup[i],temp,l,c);
    }
  }

  //return ID
  if(temp[0] == '_') {
    error("Cannot start ID with _",l,c);
  }
  if(temp[temp.length()-1] == '"') {
    error("possible missing starting quote in string: " + temp,l,c);
  }
  return Token(ID,temp,l,c);
}

#endif
