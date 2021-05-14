//----------------------------------------------------------------------
// NAME: Brennan Longstreth
// FILE: interpreter.h
// CONTRIBUTORS: Shawn Bowers
// DATE: Spring 2021
// DESC: Interpreter to run MyPL code after type checking is done.
// NOTES: 
//----------------------------------------------------------------------


#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <unordered_map>
#include <regex>
#include "ast.h"
#include "symbol_table.h"
#include "data_object.h"
#include "heap.h"


class Interpreter : public Visitor
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

  // return code from calling main
  int return_code() const;

  
private:

  // return exception
  class MyPLReturnException : public std::exception {};
  
  // the symbol table 
  SymbolTable sym_table;

  // holds the previously computed value
  DataObject curr_val;

  // the heap
  Heap heap;

  // the next oid
  size_t next_oid = 0;
  
  // the functions (all within the global environment)
  std::unordered_map<std::string,FunDecl*> functions;
  
  // the user-defined types (all within the global environment)
  std::unordered_map<std::string,TypeDecl*> types;

  // the global environment id
  int global_env_id = 0;
  
  // the program return code
  int ret_code = 0;

  // error message
  void error(const std::string& msg, const Token& token);
  void error(const std::string& msg); 
};



int Interpreter::return_code() const
{
  return ret_code;
}

void Interpreter::error(const std::string& msg, const Token& token)
{
  throw MyPLException(RUNTIME, msg, token.line(), token.column());
}


void Interpreter::error(const std::string& msg)
{
  throw MyPLException(RUNTIME, msg);
}


// TODO: finish the visitor functions
void Interpreter::visit(Program& node) { 
  // push  the  global  environment
  sym_table.push_environment ();

  //  store  the  global  envirionment  id
  global_env_id = sym_table.get_environment_id ();

  // add  the  functions  and user -defined  types
  for(Decl* d : node.decls) {
    d->accept (*this);
  }

  //  execute  the  main  function
  CallExpr  expr;
  expr.function_id = functions["main"]->id;
  expr.accept (*this);

  // pop  the  global  environment
  sym_table.pop_environment ();
}

void Interpreter::visit(FunDecl& node) { 
  //std::cout << "Function Declaration "<< node.params.size() << std::endl;
  //add function to list of functions
  functions.insert(std::pair<std::string,FunDecl*>(node.id.lexeme(),&node));
}
void Interpreter::visit(TypeDecl& node) { 
  //std::cout << "Type Declaration" << std::endl;
  //add type to list of types
  types.insert(std::pair<std::string,TypeDecl*>(node.id.lexeme(),&node));
}

// statements
void Interpreter::visit(VarDeclStmt& node) { 
  //std::cout << "Variable Declaration Statement" << std::endl;
  if(sym_table.name_exists(node.id.lexeme())) { //check for shadowing
    error(node.id.lexeme() + " already exists in this environment",node.id);
  }
  sym_table.add_name(node.id.lexeme());

  if(node.expr) {
    node.expr->accept(*this);
    //std::cout << curr_val.is_oid() << std::endl;
    sym_table.set_val_info(node.id.lexeme(),curr_val);
    //std::cout << sym_table.to_string();
  }else {
    curr_val.set_nil();
    sym_table.set_val_info(node.id.lexeme(),curr_val);
  }
}
void Interpreter::visit(AssignStmt& node) { 
  //std::cout << "Assign Statement " << node.lvalue_list.size() << std::endl;
  node.expr->accept(*this);
  if(sym_table.has_val_info(node.lvalue_list.front().lexeme())) {
    //std::cout << "Somethin" << std::endl;
    if(node.lvalue_list.size() == 1) {
      sym_table.set_val_info(node.lvalue_list.front().lexeme(),curr_val);
      //std::cout << "Assign " << node.lvalue_list.front().lexeme() << std::endl;
    }else {
      DataObject trav;
      HeapObject ho;
      size_t oid;
      //gets initial OID DataObject
      auto lvl = (node.lvalue_list.begin());
      sym_table.get_val_info(lvl->lexeme(),trav); 
      
      //std::cout << "Var named: " << lvl->lexeme() << " = " << trav.to_string() << std::endl;
      //setup for list traversal iterator, starting on second element
      lvl++;
      //while loop for traversal of loop (stops on !OID or 1 before end of list)
      
      while(lvl != --(node.lvalue_list.end())) {
        trav.value(oid);
        heap.get_obj(oid,ho);
        ho.get_val(lvl->lexeme(),trav);
        //std::cout << "using OID stored at: " << lvl->lexeme() << " = " << trav.to_string() <<std::endl;
        lvl++;
      }
      //std::cout << "using val stored at: " << lvl->lexeme() << std::endl;
      //set oid value to second to last node in the call tree value
      trav.value(oid);
      //std::cout << " - " << oid << std::endl;
      if(heap.has_obj(oid)) {
        heap.get_obj(oid,ho);
        //ho.get_val((lvl)->lexeme(),trav);
        //std::cout << lvl->lexeme() << " " << oid << std::endl;
      }else {
        error("Object missing from heap",*lvl);
      }
      //heap.print();
      //std::cout << "--------------------------" << std::endl;
      //std::cout << "cur val: " << curr_val.to_string() << std::endl;
      //std::cout << "iter val: " << lvl->lexeme() << std::endl;
      //std::cout << "stored val: " << trav.to_string() << std::endl;
      //std::cout << "has x : " << ho.has_att("x")? (lvl->lexeme() == "v") <<  << std::endl;
      ho.set_att(lvl->lexeme(),curr_val); 
      heap.set_obj(oid,ho); 
      //heap.print();
      //std::cout << "--------------------------" << std::endl;
    }
  }else {
    std::cout << "something is missing" << std::endl;
  }
  //std::cout << sym_table.to_string()<< std::endl;
}
void Interpreter::visit(ReturnStmt& node) { 
  //std::cout << "Return Statement" << std::endl;
  //  evaluate  the  expression
  node.expr ->accept (*this);
  //  throw  the  return  exception
  throw new MyPLReturnException;
}
void Interpreter::visit(IfStmt& node) { 
  //std::cout << "If Statement" << std::endl;
  node.if_part->expr->accept(*this);
  bool cond = false,flag = false;
  curr_val.value(cond);
  if(cond) {
    flag = true;
    for(Stmt*  s : node.if_part->stmts) {
      s->accept(*this);
    }
  }else {
    for(BasicIf* bi : node.else_ifs) {
      bi->expr->accept(*this);
      curr_val.value(cond);
      if(cond) {
        flag = true;
        for(Stmt* s : bi->stmts) {
          s->accept(*this);
        }
      }
    }
  }
  if(!flag) {
    for(Stmt* s : node.body_stmts) {
      s->accept(*this);
    }
  }
}
void Interpreter::visit(WhileStmt& node) {
  //std::cout << "While Statement" << std::endl;
  node.expr->accept(*this);
  bool loop_condition;
  curr_val.value(loop_condition);
  while(loop_condition) {
    sym_table.push_environment();
    for(Stmt*  s : node.stmts) {
      s->accept(*this);
    }
    sym_table.pop_environment();

    //check loop condition again
    node.expr->accept(*this);
    curr_val.value(loop_condition);
    //std::cout << loop_condition << " - loop cond \n" << sym_table.to_string() << std::endl;
  }

}
void Interpreter::visit(ForStmt& node) {
  //std::cout << "For Statement" << std::endl;
  //set up loop variable
  sym_table.push_environment();
  sym_table.add_name(node.var_id.lexeme());
  node.start->accept(*this);
  int index,end_val;
  DataObject id;
  curr_val.value(index);
  sym_table.set_val_info(node.var_id.lexeme(),curr_val);
  node.end->accept(*this);
  curr_val.value(end_val);
  while(index <= end_val) {
    //loop statements
    sym_table.push_environment();
    for(Stmt* s : node.stmts) {
      s->accept(*this);
    }
    sym_table.pop_environment();
    //increment loop var
    sym_table.get_val_info(node.var_id.lexeme(),id);
    id.value(index);
    id.set(++index);
    sym_table.set_val_info(node.var_id.lexeme(),id);
  }
  sym_table.pop_environment();
}
// expressions
void Interpreter::visit(Expr& node) {
  //std::cout << "Expression" << std::endl;
  node.first->accept(*this);
  if(node.op) {
    DataObject  lhs_val = curr_val;
    node.rest ->accept (*this);
    DataObject  rhs_val = curr_val;
    TokenType  op = node.op->type ();
    //  start  checking  various  cases (there  are  many!)
    // be sure to set  computed  value in  curr_val
    std::string operation = node.op->lexeme();
    if(lhs_val.is_nil()) {
      bool l,r;
      l = lhs_val.is_nil();
      r = rhs_val.is_nil();
      if(operation == "==") curr_val.set(r == l);
      else if (operation == "!=") curr_val.set(l != r);
      else { 
          error("Expression term is nil", node.rest->first_token());
      }
    }else if(rhs_val.is_nil()) {
      bool l,r;
      l = lhs_val.is_nil();
      r = rhs_val.is_nil();
      if(operation == "==") curr_val.set(r == l);
      else if (operation == "!=") curr_val.set(l != r);
      else { 
          error("Expression term is nil", node.rest->first_token());
      }
      
    }else if(lhs_val.is_oid()) {
      //get OIDs
      int lOID,rOID;
      if(heap.has_obj(lOID) && heap.has_obj(rOID)) {
        HeapObject l,r;
        heap.get_obj(lOID,l);
        heap.get_obj(rOID,r);
        //get the names of both
        //------------------------------------------------------------
        
        //check equal for each one
        if(operation == "==") {
          
        }else if (operation == "!=") {

        }
      }else {
        error("Object in expression not on heap",node.first_token());
      }


    }else
    if(lhs_val.is_double()) {
      if(rhs_val.is_double()) {
        double l,r;
        lhs_val.value(l);
        rhs_val.value(r);
        if(operation == "+") curr_val.set(l+r);
        else if(operation == "-") curr_val.set(l-r);
        else if(operation == "*") curr_val.set(l*r);
        else if(operation == "/") curr_val.set(l/r);
        else if(operation == "==") curr_val.set(l==r);
        else if(operation == "!=") curr_val.set(l!=r);
        else if(operation == "<") curr_val.set(l<r);
        else if(operation == "<=") curr_val.set(l<=r);
        else if(operation == ">") curr_val.set(l>r);
        else if(operation == ">=") curr_val.set(l>=r);
      }
    }else 
    if(lhs_val.is_integer()) {
      if(rhs_val.is_integer()) {
        int l,r;
        lhs_val.value(l);
        rhs_val.value(r);
        
        if(operation == "+") curr_val.set(l+r);
        else if(operation == "-") curr_val.set(l-r);
        else if(operation == "*") curr_val.set(l*r);
        else if(operation == "/") curr_val.set(l/r);
        else if(operation == "==") curr_val.set(l==r);
        else if(operation == "!=") curr_val.set(l!=r);
        else if(operation == "<") curr_val.set(l<r);
        else if(operation == "<=") curr_val.set(l<=r);
        else if(operation == ">") curr_val.set(l>r);
        else if(operation == ">=") curr_val.set(l>=r);
        else if(operation == "%") curr_val.set(l%r);
      }
    }else
    if(lhs_val.is_string()) {

      if(rhs_val.is_string()) {
        std::string l,r;
        lhs_val.value(l);
        rhs_val.value(r);
        
        if(operation == "+") curr_val.set(l+r);
        else if(operation == "==") curr_val.set(l==r);
        else if(operation == "!=") curr_val.set(l!=r);
        else if(operation == "<") curr_val.set(l<r);
        else if(operation == "<=") curr_val.set(l<=r);
        else if(operation == ">") curr_val.set(l>r);
        else if(operation == ">=") curr_val.set(l>=r);
      }else {
        std::string l;
        char r;
        lhs_val.value(l);
        rhs_val.value(r);
        if(operation == "+") curr_val.set(l+r);
      }
    }else 
    if(lhs_val.is_char()) {
      if(rhs_val.is_char()) {
        char l,r;
        lhs_val.value(l);
        rhs_val.value(r);

        if(operation == "+") curr_val.set(l+r);
        else if(operation == "==") curr_val.set(l==r);
        else if(operation == "!=") curr_val.set(l!=r);
        else if(operation == "<") curr_val.set(l<r);
        else if(operation == "<=") curr_val.set(l<=r);
        else if(operation == ">") curr_val.set(l>r);
        else if(operation == ">=") curr_val.set(l>=r);
      }else {
        char l;
        std::string r;
        lhs_val.value(l);
        rhs_val.value(r);
        if(operation == "+") curr_val.set(l+r);
      }
    }else
    if(lhs_val.is_bool() || rhs_val.is_bool()) {
      //std::cout << "Found bool" << std::endl;
      bool l,r;
      lhs_val.value(l);
      rhs_val.value(r);

      if(operation == "==") curr_val.set(l==r);
      else if(operation == "!=") curr_val.set(l!=r);
      else if(operation == "and") curr_val.set(l&&r);
      else if(operation == "or") curr_val.set(l||r);
      //std::cout << "\nCurrent Values: " << lhs_val.to_string() << " " << rhs_val.to_string() << std::endl;
    }
    else {
      std::cout << "something went wrong" << std::endl;
    }
  }
  if(node.negated) {
    
    if(curr_val.is_bool()) {
      bool val;
      curr_val.value(val);
      curr_val.set(!val);
    }else if (curr_val.is_double()) {
      double val;
      curr_val.value(val);
      curr_val.set(val * -1);
    }else if (curr_val.is_integer()) {
      int val;
      curr_val.value(val);
      curr_val.set(val * -1);
    }
  }
}
void Interpreter::visit(SimpleTerm& node) {
  //std::cout << "Simple Term " << node.first_token().line()<< std::endl;
  node.rvalue->accept(*this);
}
void Interpreter::visit(ComplexTerm& node) {
  //std::cout << "Complex Term" << std::endl;
  node.expr->accept(*this);
}
// rvalues
void Interpreter::visit(SimpleRValue& node) { 
  //std::cout << "Simple R Value" << std::endl;
  if(node.value.type() ==  CHAR_VAL)
    curr_val.set(node.value.lexeme ().at(0));
  else if(node.value.type() ==  STRING_VAL) 
    curr_val.set(node.value.lexeme ());
  else if(node.value.type() ==  INT_VAL) {
    try{curr_val.set(std::stoi(node.value.lexeme ()));
    }catch(const std::invalid_argument& e) {
      error("internal  error", node.value );
    }catch(const std::out_of_range& e) {
      error("int out of range", node.value );
    }
  }else if(node.value.type() ==  DOUBLE_VAL) {
    try{curr_val.set(std::stod(node.value.lexeme ()));
    }catch(const std::invalid_argument& e) {
      error("internal  error", node.value );
    }catch(const std::out_of_range& e) {
      error("double  out of range", node.value );
    }
  }else if(node.value.type() ==  BOOL_VAL) {
    if(node.value.lexeme () == "true")curr_val.set(true);
    else curr_val.set(false);
  }else if(node.value.type() == NIL)
    curr_val.set_nil ();
}

void Interpreter::visit(NewRValue& node) { 
  //std::cout << "New R Value" << std::endl;
  
  HeapObject ho;
  TypeDecl* td;
  if(!(types.find(node.type_id.lexeme()) == types.end())) {
    td = types[node.type_id.lexeme()];
  }
  else {
    error("type '" + node.type_id.lexeme() + "' not defined", node.type_id);
  }
  //set default values for heap object
  //std::cout << "storing '" << node.type_id.lexeme() << "' at: " << next_oid << std::endl;
  for(VarDeclStmt* v : td->vdecls) {
    DataObject d;
    v->expr->accept(*this);
    d = curr_val;
    ho.set_att(v->id.lexeme(),d);
    //std::cout << "  Var: " <<  v->id.lexeme() << " val: " << d.to_string() << std::endl;
  }
  next_oid++;
  curr_val.set(next_oid);
  heap.set_obj(next_oid,ho);
}

void Interpreter::visit(CallExpr& node) {   
  //std::cout << "Call Expression: " << node.function_id.lexeme() << std::endl;
  std:: string  fun_name = node.function_id.lexeme ();
  //  check  for built -in  functions
  if(fun_name  == "print") {
    node.arg_list.front()->accept (*this);
    std:: string s = curr_val.to_string ();
    s = std::regex_replace(s, std::regex("\\\\n"), "\n");
    s = std::regex_replace(s, std::regex("\\\\t"), "\t");
    std::cout << s; //<< std::endl;
  }else if(fun_name == "itos") {
    node.arg_list.front()->accept(*this);
    curr_val.set(curr_val.to_string());
  }else if(fun_name == "stoi") {
    node.arg_list.front()->accept(*this);
    curr_val.set(stoi(curr_val.to_string()));
  }else if(fun_name == "stod") {
    node.arg_list.front()->accept(*this);
    curr_val.set(stod(curr_val.to_string()));
  }else if(fun_name == "dtos") { //for some reason this adds a trailing 0
    node.arg_list.front()->accept(*this);
    std::string s =  curr_val.to_string();
    //remove trailing 0s
    for (int i = s.length()-1; i > 0; i--) {
      if(s[i] == '0' && s[i-1] != '.') {
        s = s.substr(0,i);
      }else {
        break;
      }
    }
    curr_val.set(s);
  }else if(fun_name == "length") {
    node.arg_list.front()->accept(*this);
    curr_val.set(curr_val.to_string().length());
  }else if(fun_name == "get") {
    node.arg_list.front()->accept(*this);
    int i;
    curr_val.value(i);
    node.arg_list.pop_front(); // CAREFUL WITH THIS, MESSES UP TREE FOR FUTURE USE.
    node.arg_list.front()->accept(*this);
    curr_val.set(curr_val.to_string().at(i));
  }else if(fun_name == "read") {
    std::string s;
    getline(std::cin, s);
    curr_val.set(s);
  }
  // etc , for  each  built in  function
  else{
    // call  the  function
    FunDecl* fun_node = functions[fun_name];
    //    1.  evaluate  the  args  and  save
    std::list<std::pair<std::string,DataObject>> args;
    auto param = fun_node->params.begin();

    //std::cout << "Function call: " << fun_name << " " << std::endl;
    for(Expr* e : node.arg_list) {
      e->accept(*this);
      DataObject d;
      d = curr_val;
      args.push_back(std::pair<std::string,DataObject>(param->id.lexeme(),d));
      //std::cout << param->id.lexeme() << "=" << d.to_string() << ", ";
      param++;
    }
    //std::cout << std::endl;

    //    2. save  the  current  environment
    int curr_environment = sym_table.get_environment_id();
    //    3. go to the  gobal  environment
    sym_table.set_environment_id(global_env_id);
    //    4. push a new  environment
    sym_table.push_environment();

    //    5. add  param  values (from 1)
    for(std::pair<std::string,DataObject> p : args) {
      sym_table.add_name(p.first);
      sym_table.set_val_info(p.first,p.second);
    }
    //std::cout << sym_table.to_string() << std::endl;
    try{
    //    6. eval  each  statement
      for(Stmt* s : fun_node->stmts) {
        s->accept(*this);
      }
    }catch(MyPLReturnException* e) {
    //    7.  catch a return  exception
    }
    //std::cout << "called: " << fun_name << " Returned: " << curr_val.to_string() << std::endl;
    //    8. pop  environment
    sym_table.pop_environment();

    //    9.  return  to  saved  environment
    sym_table.set_environment_id(curr_environment);
    
    // ...
  }
}
void Interpreter::visit(IDRValue& node) {
  //std::cout << "ID R Value "  << node.path.back().lexeme() << std::endl;
  auto t = node.path.begin();
  DataObject d;
  if(sym_table.has_val_info(t->lexeme())) {
    sym_table.get_val_info(t->lexeme(),d);
    //std::cout << "what " << d.to_string() << std::endl;
  }
  //at this point d should hold an OID;
  HeapObject ho;
  //traverse call tree till final primative type
  while ((t != --(node.path.end()) && d.is_oid())) {
    
    size_t oid;
    d.value(oid);
    //std::cout << oid << std::endl;
    if(heap.has_obj(oid)) {
      
      heap.get_obj(oid,ho);
      ho.get_val((++t)->lexeme(),d);
    }
  }
  //check primative type
  
  //std::cout << "got here" << std::endl;
  ho.get_val(t->lexeme(),d);
  
  curr_val = d;
 //std::cout << sym_table.to_string() << std::endl;
}
void Interpreter::visit(NegatedRValue& node) {
  //probably did something wrong but this never gets called from what i can tell
  // and everything works fine.
  //std::cout << "Negated R Value" << std::endl;
}

#endif
