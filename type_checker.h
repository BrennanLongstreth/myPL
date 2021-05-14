//----------------------------------------------------------------------
// NAME: 
// FILE: 
// DATE: 
// DESC:
//----------------------------------------------------------------------


#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include <iterator>
#include <iostream>
#include "ast.h"
#include "symbol_table.h"


class TypeChecker : public Visitor
{
public:

  // top-level
  void visit(Program& node);
  void visit(FunDecl& node);
  void visit(TypeDecl& node);
  // statements
  void visit(VarDeclStmt& node);
  void visit(AssignStmt& node);
  void visit(ReturnStmt& node);
  void visit(IfStmt& node);
  void visit(WhileStmt& node);
  void visit(ForStmt& node);
  // expressions
  void visit(Expr& node);
  void visit(SimpleTerm& node);
  void visit(ComplexTerm& node);
  // rvalues
  void visit(SimpleRValue& node);
  void visit(NewRValue& node);
  void visit(CallExpr& node);
  void visit(IDRValue& node);
  void visit(NegatedRValue& node);

private:

  // the symbol table 
  SymbolTable sym_table;

  // the previously inferred type
  std::string curr_type;

  // helper to add built in functions
  void initialize_built_in_types();

  // error message
  void error(const std::string& msg, const Token& token);
  void error(const std::string& msg); 

  //checks if operator is legal for type
  bool is_legal_op(std::string op,std::string type,std::string rhs);

  //takes type int and puts it to a string
  std::string typeToString(int t);
};


void TypeChecker::error(const std::string& msg, const Token& token)
{
  throw MyPLException(SEMANTIC, msg, token.line(), token.column());
}


void TypeChecker::error(const std::string& msg)
{
  throw MyPLException(SEMANTIC, msg);
}


void TypeChecker::initialize_built_in_types()
{
  // print function
  sym_table.add_name("print");
  sym_table.set_vec_info("print", StringVec {"string", "nil"});
  // stoi function
  sym_table.add_name("stoi");
  sym_table.set_vec_info("stoi", StringVec {"string", "int"});  

  // TODO: finish the rest of the built-in functions: stod, itos,
  // dtos, get, length, and read
  sym_table.add_name("stod");
  sym_table.set_vec_info("stod", StringVec {"string", "double"}); 

  sym_table.add_name("itos");
  sym_table.set_vec_info("itos", StringVec {"int", "string"}); 

  sym_table.add_name("dtos");
  sym_table.set_vec_info("dtos", StringVec {"double", "string"}); 

  sym_table.add_name("get");
  sym_table.set_vec_info("get", StringVec {"int", "string", "char"}); 

  sym_table.add_name("length");
  sym_table.set_vec_info("length", StringVec {"string", "int"}); 

  sym_table.add_name("read");
  sym_table.set_vec_info("read", StringVec {"string"}); 
}


void TypeChecker::visit(Program& node)
{
  // push the global environment
  sym_table.push_environment();
  // add built-in functions
  initialize_built_in_types();
  // push 
  for (Decl* d : node.decls)
    d->accept(*this);
  // check for a main function
  if (sym_table.name_exists("main") && sym_table.has_vec_info("main")) {
    StringVec sv;
    sym_table.get_vec_info("main",sv);
    if(sv.size() != 1) {
      error("incorrect number of arguments for 'main'");
    }
    if(sv.front() != "int") {
      error("main function must return 'int'");
    }
  }
  else {
    // NOTE: the only time the 1-argument version of error should be
    // called!
    error("undefined 'main' function");
  }
   // pop the global environment
  sym_table.pop_environment();
  std::cout << "Finished type checking" << std::endl;
}


//map is for types
//vec is for func
//string is for var

// TODO: Implement the remaining visitor functions

// top-level
//void TypeChecker::visit(Program& node) {}

void TypeChecker::visit(FunDecl& node) {  
  //std::cout << node.stmts.size() << std::endl;
  sym_table.add_name(node.id.lexeme());
  StringVec s;
  //push fun environment
  sym_table.push_environment();
  //push params to the current environment
  for(FunDecl::FunParam f : node.params) {
    //add to function definition
    s.push_back(f.type.lexeme());
    //difine variables for inside function
    sym_table.add_name(f.id.lexeme());
    sym_table.set_str_info(f.id.lexeme(),f.type.lexeme());
  }
  //deal with return
  sym_table.add_name("return");
  sym_table.set_str_info("return",node.return_type.lexeme());
  s.push_back(node.return_type.lexeme());
  //define vec info for function
  sym_table.set_vec_info(node.id.lexeme(), s);
  //body statements
  for(Stmt* st : node.stmts) {
    st->accept(*this);
  }
  //pop fun environment
  sym_table.pop_environment();
  
  //std::cout << "Finished defining function:\n" << sym_table.to_string() << std::endl;
}

void TypeChecker::visit(TypeDecl& node) {
  if(sym_table.name_exists(node.id.lexeme())) {
    error("Type already declared", node.id);
  }
  sym_table.add_name(node.id.lexeme());
  StringMap s;
  sym_table.push_environment();
  for(VarDeclStmt* v : node.vdecls) {
    //uses vardeclstmt visitor to check to make sure they are proper decl
    v->accept(*this);
    //i assume you cannot put functions in UDTs
    std::pair<std::string,std::string> p;
    p.first = v->id.lexeme();
    p.second = curr_type;
    s.insert(p);
  }
  sym_table.pop_environment();
  sym_table.set_map_info(node.id.lexeme(), s);
  //std::cout << "Finished defining type:\n" << sym_table.to_string() << std::endl;
}


// statements
void TypeChecker::visit(VarDeclStmt& node) {
  //check that id isnt already in the table
  if(!sym_table.name_exists_in_curr_env(node.id.lexeme())) {
    curr_type = ""; //no type selected
    //send to expression to get expression type for type inference
    if(node.expr) node.expr->accept(*this);
    //checks that expression type matches the type of the variable
    if(node.type && (node.type->lexeme() != curr_type && curr_type != "nil")) {
      error("Type mismach in var declaration between " + node.type->lexeme() + " and " + curr_type, node.id);
    } 
    
    //add the var to the symbol table
    sym_table.add_name(node.id.lexeme());
    if(node.type) {
      sym_table.set_str_info(node.id.lexeme(),node.type->lexeme());
      curr_type = node.type->lexeme();
    }else {
      sym_table.set_str_info(node.id.lexeme(),curr_type);
    }

    //set tree data to match type inference
    if(!node.type) {
      node.type = new Token(NIL,curr_type,0,0);
      node.type->setType(curr_type);
    }
  }else {
    error("There is already a variable with this name in the current environment", node.id);
  }
  //std::cout << "Finished defining var:\n" << sym_table.to_string() << std::endl;
}

void TypeChecker::visit(AssignStmt& node) {
  //std::cout << "Assign " << node.lvalue_list.front().lexeme() << std::endl;
  //sets left hand side type to initial left hand side
  std::string lhs_type = typeToString(node.lvalue_list.front().type()); 
  //loop traverses call tree
  auto t = node.lvalue_list.begin();
  std::string temp;
  //check rhs
  curr_type = "";
  if(node.expr) { //expr should exist
    node.expr->accept(*this);
  }else {
    std::cout << "unexpected result 3" << std::endl;
  }
  //loop
  if(sym_table.has_str_info(t->lexeme())) {
    sym_table.get_str_info(t->lexeme(),temp);
    if(temp == "nil") {
      sym_table.set_str_info(t->lexeme(), curr_type);
    }
  }
  //std::cout << temp << std::endl;
  while (t != --(node.lvalue_list.end())) {
    if(sym_table.has_map_info(temp)) {
      //get map info
      std::map<std::string,std::string> curr_map;
      sym_table.get_map_info(temp,curr_map);

      //get value from map at ++t->lexeme()
      temp = curr_map[(++t)->lexeme()];
      //std::cout << temp << std::endl;
    }else {
      error(t->lexeme() + " not found",*t);
    }
  }
  lhs_type = temp;
  //check that types allign
  if(lhs_type != curr_type && curr_type != "nil") {
    error("Type mismatch between " + lhs_type + " and " + curr_type , node.expr->first_token());
  }
}

void TypeChecker::visit(ReturnStmt& node) {
  node.expr->accept(*this);
  std::string s;
  sym_table.get_str_info("return",s);
  if(curr_type != s) {
    error("Type mismatch in return expression, expecting " + s + " recieved " + curr_type, node.expr->first_token());
  }
}

void TypeChecker::visit(IfStmt& node) {
  //conditional 
  sym_table.push_environment();
  curr_type = "";
  node.if_part->expr->accept(*this);
  if(curr_type != "bool") {error("Expecting bool expression",node.if_part->expr->first_token());}
  //if body
  for(Stmt* s : node.if_part->stmts) {
    s->accept(*this);
  }
  sym_table.pop_environment();

  //else ifs
  for(BasicIf* bi : node.else_ifs) {
    sym_table.push_environment();
    //conditional
    curr_type = "";
    bi->expr->accept(*this);
    if(curr_type != "bool") {error("Expecting bool expression", bi->expr->first_token());}
    //else if body
    
    for(Stmt* s : bi->stmts) {
      s->accept(*this);
    }
    sym_table.pop_environment();
  }

  //else
  if(node.body_stmts.size() > 0) {
    sym_table.push_environment();
    for (Stmt* s : node.body_stmts) {
      s->accept(*this);
    }
    sym_table.pop_environment();
  }
}
void TypeChecker::visit(WhileStmt& node) {
  sym_table.push_environment();
  curr_type = "";
  node.expr->accept(*this);
  if(curr_type != "bool") {error("Expecting bool expression",node.expr->first->first_token());}

  
  for(Stmt * s : node.stmts) {
    s->accept(*this);
  }
  sym_table.pop_environment();
}

void TypeChecker::visit(ForStmt& node) {
  sym_table.push_environment();
  //checking if id exists and changes its type to int
  //if it doesnt exist, creates new var and sets its type to int
  if(sym_table.has_str_info(node.var_id.lexeme())) {
    sym_table.set_str_info(node.var_id.lexeme(),"int");
  }else {
    sym_table.add_name(node.var_id.lexeme());
    sym_table.set_str_info(node.var_id.lexeme(),"int");
  }
  node.start->accept(*this);
  if(curr_type != "int") {
    error("Expecting 'int' in start expression",node.start->first_token());
  }
  node.end->accept(*this);
  if(curr_type != "int") {
    error("Expecting 'int' in end expression",node.end->first_token());
  }
  //loop body
  for(Stmt* s : node.stmts) {
    s->accept(*this);
  }
  sym_table.pop_environment();
}


// expressions
void TypeChecker::visit(Expr& node) {
  //std::cout << "entered expr " << node.first_token().lexeme() << std::endl;
  std::string lhs = "";
  if(node.first) {
    node.first->accept(*this);
    lhs = curr_type;
  }
  //std::cout << "lhs = " << lhs << " Curr_type = " << curr_type << std::endl;
  //checking for illegal operators for type
  if(node.rest) {
    //std::cout << "rest --------" << std::endl;
    node.rest->accept(*this);
  }


  if(node.op) {
    if(!is_legal_op(node.op->lexeme(),lhs,curr_type)) {
      error("Illegal operator \'" + node.op->lexeme() + "\' for " + lhs + " and " + curr_type, *node.op);
    }else 
    //checking operator rules
    if(lhs == "string" && curr_type == "char") {
      curr_type = "string";
    }else if(lhs == "char" && curr_type == "char" && node.op->lexeme() == "+") {
      curr_type = "string";
    }else if(lhs == "double" && curr_type == "int") {
      curr_type = "double";
    }
    
    //checks if type needs to be bool
    if(node.op->lexeme() == "==" || node.op->lexeme() == "!=" 
            || node.op->lexeme() == ">" || node.op->lexeme() == "<" || node.op->lexeme() == ">=" || node.op->lexeme() == "<="){ 
      curr_type = "bool";     
    }
  }
  if(node.negated && (curr_type != "bool" && curr_type != "int" && curr_type != "double")) {
    error("Expecting bool or int or double expression, recieved: " + curr_type, node.first_token());
  }




  //check udt
  //check primative types
}

//helper to find legal operations
bool TypeChecker::is_legal_op(std::string op,std::string lhs,std::string rhs) {
  //std::cout << op << lhs << rhs << std::endl;
  if(lhs == "nil" || rhs == "nil") { //not sure if this needs to be checked
    if(op == "==" || op == "!=") { 
      return true;
    }else {
      return false;
    }
  }
  if(lhs == "bool" && rhs == "bool") {
    if(op == "==" || op == "!=" || op == "and" || op == "or") {
      return true;
    }else {
      return false;
    }
  }else if((lhs == "string" || lhs == "char") && (rhs == "string" || rhs == "char")) {
    if(op == "+") {
      return true;
    }
    if(lhs == rhs && (op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" || op == "<=")) {
      return true;
    }else {
      return false;
    }
  }else if((lhs == "int" || lhs == "double") && lhs == rhs) {
    if(lhs == "int" && op == "%") {
      return true;
    }
    if(op == "+" || op == "-" || op == "/" || op == "*" || op == "==" || op == "!=" 
        || op == ">" || op == "<" || op == ">=" || op == "<=") {
      return true;
    }else {
      return false;
    }
  }else if(lhs == "int" || lhs == "double") {
    //if(op == "+" || op == "-" || op == "/" || op == "*" || op == "%" || op == "==" || op == "!=" 
    //    || op == ">" || op == "<" || op == ">=" || op == "<=") {
    //  return true;
    //}else {
      return false;
    //}
  }else { //udt
    if(op == "==" || op == "!=") { 
      return true;
    }else {
      return false;
    }
  }
}

void TypeChecker::visit(SimpleTerm& node) {
  node.rvalue->accept(*this);
}
void TypeChecker::visit(ComplexTerm& node) {
  node.expr->accept(*this);
}

// rvalues
void TypeChecker::visit(SimpleRValue& node) {
  //std::cout << "SimpleRValue: " << node.value.lexeme() << " " << node.value.type() << std::endl;
  curr_type = typeToString(node.value.type()); 
}

void TypeChecker::visit(NewRValue& node) {
  //std::cout << node.type_id.lexeme() << std::endl;
  curr_type = node.type_id.lexeme();
}

void TypeChecker::visit(CallExpr& node) {
  //std::cout << "call" << std::endl;
  if(sym_table.has_vec_info(node.function_id.lexeme())) {
    StringVec sv;
    sym_table.get_vec_info(node.function_id.lexeme(), sv);
    std::string temp = sv.back();
    sv.pop_back();
    if(node.arg_list.size() < sv.size()) {
      error("Too few arguments in function call", node.function_id);
    }else if(node.arg_list.size() > sv.size()) {
      error("Too many arguments in function call", node.function_id);
    }
    auto vecData = sv.begin();
    for(Expr* e : node.arg_list) {
      //std::cout << node.arg_list.front()->first->first_token().lexeme() << std::endl;
      e->accept(*this);
      if(curr_type != *vecData){
        error("Mismatched types in function call, expecting " + sv.front(),e->first_token());
      }
      vecData++;
    }
    
    curr_type = temp;
  }else {
    error("Missing function definition for " + node.function_id.lexeme(), node.function_id);
  }
}

void TypeChecker::visit(IDRValue& node) {
  //check if start has string info,
  auto t = node.path.begin();
  std::string temp;
  //loop
  if(sym_table.has_str_info(t->lexeme())) {
    sym_table.get_str_info(t->lexeme(),temp);
  }
  //std::cout << temp << std::endl;
  while (t != --(node.path.end())) {
    if(sym_table.has_map_info(temp)) {
      //get map info
      std::map<std::string,std::string> curr_map;
      sym_table.get_map_info(temp,curr_map);

      //get value from map at ++t->lexeme()
      temp = curr_map[(++t)->lexeme()];
      //std::cout << temp << std::endl;
    }else {
      error(t->lexeme() + " not found",*t);
    }
  }
  curr_type = temp;
}

void TypeChecker::visit(NegatedRValue& node) {
  std::cout << "Negative" << std::endl;
}

std::string TypeChecker::typeToString(int t) {
  //std::cout << "called type to string" << std::endl;
  switch(t) {
    case 40: return "bool";
    case 41: return "int";
    case 42: return "double";
    case 43: return "string";
    case 44: return "char";
    case 45: return "id";
    case 46: return "nil";
    default: return "WHAT";
  }
}
#endif
