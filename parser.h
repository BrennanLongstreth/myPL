//----------------------------------------------------------------------
// NAME: Brennan Longstreth
// FILE: parser.h
// CONTRIBUTORS: Shawn Bowers
// DATE: 3/1/21
// DESC: Uses tokens from the lexer to build an abstract syntax tree.
// NOTES: 
//----------------------------------------------------------------------

#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "mypl_exception.h"
#include "ast.h"


class Parser 
{
public:

  // create a new recursive descent parser
  Parser(const Lexer& program_lexer);

  // run the parser
  void parse(Program& prog);
  
private:
  Lexer lexer;
  Token curr_token;

  bool _debug = false;
  
  // helper functions
  void advance();
  void eat(TokenType t, std::string err_msg);
  void error(std::string err_msg);
  bool is_operator(TokenType t);
  
  // recursive descent functions
  void tdecl(TypeDecl& node);
  void fdecl(FunDecl& node);
  // TODO: add additional recursive descent functions here
  //void vdecls(); //
  void params(FunDecl& node); //
  Token dtype(); //
  void stmts(std::list<Stmt*>& s); //
  bool stmt(std::list<Stmt*>& s); //

  void vdecl_stmt(VarDeclStmt& node); //

  void assign_stmt(AssignStmt& node);
  void lvalue(AssignStmt& node);

  void cond_stmt(IfStmt& node);
  void condt(IfStmt& node);

  void while_stmt(WhileStmt& node);

  void for_stmt(ForStmt& node);

  void call_expr(CallExpr& node);
  void args(std::list<Expr*>& e);

  void exit_stmt(ReturnStmt& node);
  void expr(Expr& e);

  void is_operator();

  void rvalue(SimpleTerm& node);
  void pval();
  void idrval(IDRValue& i);

  bool is_pval(TokenType);
};


// constructor
Parser::Parser(const Lexer& program_lexer) : lexer(program_lexer)
{
}


// Helper functions

void Parser::advance()
{
  curr_token = lexer.next_token();
}


void Parser::eat(TokenType t, std::string err_msg)
{
  if (curr_token.type() == t)
    advance();
  else
    error(err_msg);
}


void Parser::error(std::string err_msg)
{
  std::string s = err_msg + "found '" + curr_token.lexeme() + "'";
  int line = curr_token.line();
  int col = curr_token.column();
  throw MyPLException(SYNTAX, s, line, col);
}


bool Parser::is_operator(TokenType t)
{
  return t == PLUS or t == MINUS or t == DIVIDE or t == MULTIPLY or
    t == MODULO or t == AND or t == OR or t == EQUAL or t == LESS or
    t == GREATER or t == LESS_EQUAL or t == GREATER_EQUAL or t == NOT_EQUAL;
}

// Recursive-decent functions

void Parser::parse(Program& prog)
{
  if(_debug)std::cout << "Starting Parser" << std::endl;
  advance();
  while (curr_token.type() != EOS) {
    if (curr_token.type() == TYPE) {
      TypeDecl* d = new TypeDecl;
      tdecl(*d);
      prog.decls.push_back(d);
    }else{
      FunDecl* d = new FunDecl;
      fdecl(*d);
      prog.decls.push_back(d);
    }      
  }
  eat(EOS, "expecting end-of-file ");
}


void Parser::tdecl(TypeDecl& node) 
{
  eat(TYPE, "expecting 'type' ");
  node.id = curr_token;
  eat(ID, "expecting ID ");
  //replaces vdecls recursion to allow for loop definition outside of vdecl_stmt()
  std::list<VarDeclStmt*>* vs = new std::list<VarDeclStmt*>();
  while(curr_token.type() == VAR) {
    VarDeclStmt* vd = new VarDeclStmt;
    vdecl_stmt(*vd);
    vs->push_back(vd);
  }
  node.vdecls = *vs;
  eat(END, "Expecting 'end' ");
}

void Parser::fdecl(FunDecl& node) 
{
  eat(FUN, "expecting 'fun' ");
  node.return_type = curr_token;
  if(curr_token.type() != NIL) {
    dtype(); //can catch ID if type is missing
  }else {
    eat(NIL, "expecting 'nil' ");
  }
  node.id = curr_token;
  eat(ID, "expecting ID ");
  eat(LPAREN, "expecting '(' ");
  params(node);
  eat(RPAREN, "expecting ')' ");
  stmts(node.stmts);
  eat(END, "expecting 'end' ");
}


void Parser::params(FunDecl& node) {
  if(curr_token.type() == ID) {
    FunDecl::FunParam f;
    f.id = curr_token;
    advance(); //already checked for ID
    eat(COLON, "expecting ':' ");
    f.type = curr_token;
    dtype();
    node.params.push_back(f);
    if(curr_token.type() == COMMA) {
      advance();
      params(node);
    }
  }
  //empty
}

Token Parser::dtype() { 
  TokenType t = curr_token.type();
  if(t == INT_TYPE || t == DOUBLE_TYPE || t == BOOL_TYPE || t == CHAR_TYPE || t == STRING_TYPE || t == ID) {
    Token t = curr_token;
    advance();
  }else {
    error("expecting primative type ");
  }
  return curr_token;
}

void Parser::stmts(std::list<Stmt*>& s) {
  if(stmt(s)) {
    stmts(s);
  }
  //empty
}

bool Parser::stmt(std::list<Stmt*>& s) {
  switch(curr_token.type()) {
    case VAR: {
      VarDeclStmt* v = new VarDeclStmt;
      vdecl_stmt(*v); 
      s.push_back(v);
      }
      break;//vdecl statement
    case ID: { 
      Token temp = curr_token; //saves ID
      advance(); //REMEMBER: for call and lvalue first token is already used;
      if(curr_token.type() == LPAREN) {
        CallExpr* c = new CallExpr;
        c->function_id = temp;
        call_expr(*c);
        s.push_back(c);
      }else if(curr_token.type() == ASSIGN || curr_token.type() == DOT) {
        AssignStmt* a = new AssignStmt;
        a->lvalue_list.push_back(temp);
        assign_stmt(*a);
        s.push_back(a);
      }else {
        error("Expecting '(' or '=' or '.' ");
      }
      break;
    }//assign statement/call expression
    case IF: {
        IfStmt* i = new IfStmt();
        cond_stmt(*i); 
        s.push_back(i);
      }
      break;//cond statement
    case WHILE: {
        WhileStmt* w = new WhileStmt();
        while_stmt(*w);
        s.push_back(w);
      }
      break;//while statement
    case FOR: {
        ForStmt* f = new ForStmt();
        for_stmt(*f); 
        s.push_back(f);
      }
      break;//for statement
    case RETURN: {
        ReturnStmt* r = new ReturnStmt();
        exit_stmt(*r); 
        s.push_back(r);
      }
      break;//exit statement
    default: return false;
  }
  return true;
}

void Parser::vdecl_stmt(VarDeclStmt& node) { 
  advance(); //already checked for var before entering
  node.id = curr_token;
  eat(ID, "expecting ID ");
  if(curr_token.type() == COLON) {
    advance(); //already checked colon
    Token* temp = new Token(); 
    *temp = curr_token;
    node.type = temp;
    //if(_debug) std::cout << node.type->lexeme() << std::endl;
    dtype();
  }else if(curr_token.type() != ASSIGN) {
    error("expecting COLON or '=' ");
  }
  eat(ASSIGN, "expecting '=' ");
  Expr* e = new Expr();
  expr(*e);
  node.expr = e;
}

void Parser::assign_stmt(AssignStmt& node) {
  //ID already taken
  lvalue(node);
  eat(ASSIGN, "expecting '=' ");
  Expr* e = new Expr();
  expr(*e);
  node.expr = e;
}

void Parser::lvalue(AssignStmt& node) {
  //id already pulled when deciding on statement type
  while (curr_token.type() == DOT) {
    advance(); // type already checked
    node.lvalue_list.push_back(curr_token);
    eat(ID, "expecting ID ");
  }
  //empty
}

void Parser::cond_stmt(IfStmt& node) {
  advance(); //already checked for if while in stmt
  BasicIf* b = new BasicIf;
  Expr* e = new Expr;
  expr(*e);
  b->expr = e;
  node.if_part = b;
  eat(THEN, "expecting 'then' ");
  stmts(b->stmts);
  condt(node);
  eat(END, "expecting 'end' ");
}

void Parser::condt(IfStmt& node) {
  if(curr_token.type() == ELSEIF) {
    BasicIf* b = new BasicIf;
    advance(); //already checked for ELSEIF
    Expr* e = new Expr();
    expr(*e);
    b->expr = e;
    eat(THEN, "expecting 'then' ");
    stmts(b->stmts);
    condt(node);
    node.else_ifs.push_back(b);
  }else if(curr_token.type() == ELSE) {
    advance(); //already checked for ELSE
    stmts(node.body_stmts);
  }
  //empty
}

void Parser::while_stmt(WhileStmt& node) {
  advance(); //already checked for WHILE
  Expr* e = new Expr;
  expr(*e);
  node.expr = e;
  eat(DO, "expecting 'do' ");
  stmts(node.stmts);
  eat(END, "expecting 'end' ");
}

void Parser::for_stmt(ForStmt& node) {
  advance(); //already checked for FOR
  node.var_id = curr_token;
  eat(ID, "expecting ID ");
  eat(ASSIGN, "expecting '=' ");
  Expr* e1 = new Expr;
  expr(*e1);
  node.start = e1;
  eat(TO, "expecting 'to' ");
  Expr* e2 = new Expr;
  expr(*e2);
  node.end = e2;
  eat(DO, "expecting 'do' ");
  stmts(node.stmts);
  eat(END, "expecting 'end' ");
}

void Parser::call_expr(CallExpr& node) {
  //ID already eaten
  eat(LPAREN, "expecting '(' ");
  args(node.arg_list);
  eat(RPAREN, "expecting ')' ");
}

void Parser::args(std::list<Expr*>& e) {
  if(curr_token.type() != RPAREN) {
    Expr* ex = new Expr;
    expr(*ex);
    e.push_back(ex);
    while(curr_token.type() == COMMA) {
      Expr* exp = new Expr;
      advance();
      expr(*exp);
      e.push_back(exp);
    }
  }
  //empty
}

void Parser::exit_stmt(ReturnStmt& node) {
  advance();
  Expr* e = new Expr();
  expr(*e);
  node.expr = e;
}

void Parser::expr(Expr& e) {
  if(curr_token.type() == NOT || curr_token.type() == NEG) {
    e.negated = true;
    advance();
    expr(e);
  }else if(curr_token.type() == LPAREN) {
    ComplexTerm* ct = new ComplexTerm;
    Expr* ex = new Expr();
    advance();
    expr(*ex);
    ct->expr = ex;
    e.first = ct;
    eat(RPAREN, "expecting ')' ");
  }else {
    SimpleTerm* s = new SimpleTerm();
    rvalue(*s);
    e.first = s;
  }
  if(is_operator(curr_token.type())) {
    
    Expr* ex = new Expr();
    Token* tempOP = new Token();
    *tempOP = curr_token;
    e.op = tempOP;
    advance();
    expr(*ex);
    e.rest = ex;
  }else {
    //empty
  }
}


void Parser::rvalue(SimpleTerm& node) {
  if(_debug) std::cout << " PARSE SIMPLE TERM " << std::endl;
  TokenType t = curr_token.type();
  if(is_pval(t)) {
    if(_debug) std::cout << curr_token.lexeme() << std::endl;
    SimpleRValue* s = new SimpleRValue;
    s->value = curr_token;
    advance();
    node.rvalue = s;
  }else if(t == NIL) {
    SimpleRValue* s = new SimpleRValue;
    s->value = curr_token;
    advance();
    node.rvalue = s;
  }else if(t == NEW) {
    NewRValue* n = new NewRValue;
    advance();
    n->type_id = curr_token;
    eat(ID, "expecting ID ");
    node.rvalue = n;
  }else if(t == ID) {
    Token temp = curr_token;
    advance();
    if(curr_token.type() == LPAREN) {
      CallExpr* c = new CallExpr;
      c->function_id = temp;
      call_expr(*c);
      node.rvalue = c;
    }else {
      IDRValue* i = new IDRValue;
      i->path.push_back(temp);
      idrval(*i);
      node.rvalue = i;
    }
  }else if(t == NEG) {
    NegatedRValue* n = new NegatedRValue;
    advance();
    Expr* e= new Expr;
    expr(*e);
    n->expr = e;
    node.rvalue = n;
  }else {
    error("Invalid expression ");
  }
}

bool Parser::is_pval(TokenType t) {
  return (t == INT_VAL || t == DOUBLE_VAL || t == BOOL_VAL || t == CHAR_VAL || t == STRING_VAL);
}

void Parser::idrval(IDRValue& i) {
  while(curr_token.type() == DOT) {
    advance(); //already checked for DOT
    i.path.push_back(curr_token);
    eat(ID, "expecting ID ");
  }
}

#endif
