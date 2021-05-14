//----------------------------------------------------------------------
// NAME: Brennan Longstreth
// FILE: printer.h
// CONTRIBUTORS: Shawn Bowers
// DATE: 3/1/21
// DESC: translates the program to c++ and places it in test.cpp
// NOTES: Cannot replicate behavior around setting '=' to nil on non UDT 
//        types
//----------------------------------------------------------------------

#ifndef PRINTER_H
#define PRINTER_H

#include <iostream>
#include "ast.h"
#include <typeinfo>


class Printer : public Visitor
{
public:
  // constructor
  Printer(std::ostream& output_stream) : out(output_stream) {}

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
  std::ostream& out;
  int indent = 0;

  bool _debug = false;

  void inc_indent() {indent += 3;}
  void dec_indent() {indent -= 3;}
  std::string get_indent() {return std::string(indent, ' ');}
  
  std::list<std::string> types;
};


// TODO: Implement the visitor functions 
void Printer::visit(Program& node) {
  if(_debug) std::cout << "Starting Visitor" << std::endl;
  out << "// AUTO GENERATED CODE: \n// Translator written by Brennan Longstreth\n\n#include <iostream>\n#include \"built_in_fun.h\"\n\nusing namespace std;\n\n";
  for(Decl* d : node.decls){
    d->accept(*this);
    out << std::endl;
  }
}


void Printer::visit(FunDecl& node) {
  if(_debug) std::cout << " VISIT FUNC DECL" << std::endl;
//prints first line of function declaration
  if(node.return_type.lexeme() == "nil") {
    out << get_indent() << "void " << node.id.lexeme() << "(";
  }else  if(std::find(types.begin(),types.end(),node.return_type.lexeme()) != types.end()) {
    out << get_indent() << "" << node.return_type.lexeme() << "* " << node.id.lexeme() << "(";
  }else {
    out << get_indent() << "" << node.return_type.lexeme() << " " << node.id.lexeme() << "("; 
  }

  

  for(FunDecl::FunParam f : node.params) {
    if(std::find(types.begin(),types.end(),f.type.lexeme()) != types.end()) {
      out << f.type.lexeme() << "* " << f.id.lexeme();
    }else {
      out << f.type.lexeme() << " " << f.id.lexeme();
    }
    
    if(f.id.lexeme() != node.params.back().id.lexeme()) 
        out << ", ";
  }
  out << ") {\n";

//prints body of function declaration
  inc_indent();
  for(Stmt* s : node.stmts) {
    out << get_indent();
    s->accept(*this);
    out << ";\n";
  }
  dec_indent();
//prints end
  out << get_indent() << "}" << std::endl;
}


void Printer::visit(TypeDecl& node) {
  if(_debug) std::cout << " VISIT TYPE DECL " << node.vdecls.size() << std::endl;
//prints first line of type def
  out << get_indent() << "struct " << node.id.lexeme() << " {\n";
  //adding type to list of types
  types.push_back(node.id.lexeme());
//prints var decl statements
  inc_indent();
  for(VarDeclStmt* v : node.vdecls) {
    out << get_indent();
    v->accept(*this);
    out << ";\n";
  }
  dec_indent();
//prints end
  out << get_indent() << "};" << std::endl;
}



// statements
void Printer::visit(VarDeclStmt& node) {
  if(_debug) std::cout << " VISIT VAR DECL" << std::endl;
//print lhs
  if(node.type) {
    out <<  node.type->lexeme();
    //std::cout << node.id.lexeme() << " " << node.expr->first->first_token().to_string() << std::endl; 
    if (node.expr->first->first_token().type() == NIL ||
        (node.expr->first->first_token().type() == ID && 
         !(node.expr->op) && std::find(types.begin(),types.end(),node.expr->first->first_token().lexeme()) != types.end())
        || std::find(types.begin(),types.end(),node.type->lexeme()) != types.end()) {
      out << "*";
      
    }
    out << " " << node.id.lexeme();
  }else {
    out << node.id.lexeme() << " <- NEEDS TYPE";
  }
//print rhs
  if(node.expr) {
    out << " = ";
    node.expr->accept(*this);
  }
}


void Printer::visit(AssignStmt& node) {
  if(_debug) std::cout << " VISIT ASSIGN " << std::endl;
  //print lhs
  while(node.lvalue_list.size() > 1) {
    out << node.lvalue_list.front().lexeme() << "->";
    node.lvalue_list.pop_front(); //sets up for next visit
  }
  //if required to not print extra dot at the end of list
  out << node.lvalue_list.front().lexeme();
  node.lvalue_list.pop_front(); //trying to reduce memory leaks
//print =
  out << " = ";
//print rhs
  if(node.expr) {
    node.expr->accept(*this);
  }
}

void Printer::visit(ReturnStmt& node) {
  if(_debug) std::cout << " VISIT RETURN " << std::endl;
  out << "return";
  if(node.expr->first->first_token().type() != NIL) {
    out << " ";
    node.expr->accept(*this);
  }
  
}

void Printer::visit(IfStmt& node) {
  if(_debug) std::cout << " VISIT IF " << std::endl;
//print first line
  out << "if ";
  node.if_part->expr->accept(*this);
  out << " {\n";
//print body
  //first body
  inc_indent();
  for(Stmt* s : node.if_part->stmts) {
    out << get_indent();
    s->accept(*this);
    out << ";\n";
  }
  dec_indent();
  if(node.if_part->stmts.size() > 0) {
    out << get_indent() << "}";
  }
  
  //else if
  
  for(BasicIf* e : node.else_ifs) {
    out  << " else if ";
    e->expr->accept(*this);
    out << " {\n";
    inc_indent();
    for(Stmt* s : e->stmts) {
      out << get_indent();
      s->accept(*this);
      out << ";\n";
    }
    dec_indent();
    out << get_indent() << "}";
  }
  //else
  if(node.body_stmts.size() > 0) {
    out << " else {\n";
    inc_indent();
    for(Stmt* s : node.body_stmts) {
      out << get_indent();
      s->accept(*this);
      out << ";\n";
    }
    dec_indent();
    out << get_indent() << "}";
  }
  
}

void Printer::visit(WhileStmt& node) {
  if(_debug) std::cout << " VISIT WHILE " << std::endl;
  out << "while ";
  if(node.expr->negated) {
    out << "!";
  }
  node.expr->accept(*this);
  out << " {\n";
  inc_indent();
  for(Stmt* s : node.stmts) {
    out << get_indent();
    s->accept(*this);
    out << ";\n";
  }
  dec_indent();
  out << get_indent() << "}";
}

void Printer::visit(ForStmt& node) {
  if(_debug) std::cout << " VISIT FOR " << std::endl;
  out << "for (int " << node.var_id.lexeme() << " = ";
  node.start->accept(*this);
  out << "; " << node.var_id.lexeme() << " <= ";
  if(node.end->negated) {
    out << " neg ";
  }
  node.end->accept(*this);
  out << "; " << node.var_id.lexeme() <<"++) {\n";
  inc_indent();
  for(Stmt* s : node.stmts) {
    out << get_indent();
    s->accept(*this);
    out << ";\n";
  }
  dec_indent();
  out << get_indent() << "}";
}

// expressions
void Printer::visit(Expr& node) {
  if(_debug) std::cout << " VISIT EXPR " << std::endl;
  if(node.negated) {
    out << "(!";
  }
  if((node.first && node.op && node.rest) || node.first->first_token().type() == BOOL_VAL) {
    out << "(";
  }
  if(node.first->first_token().type() == STRING_VAL && 
    (node.op && node.op->type() == PLUS)) {
    out << "string(";
  }
  if(node.first) {
    node.first->accept(*this);
  }
  if(node.first->first_token().type() == STRING_VAL &&
    (node.op && node.op->type() == PLUS)) {
    out << ")";
  }
  if(node.op) {
    if(node.op->type() == OR) {
      out << " || ";
    }else if (node.op->type() == AND) {
      out << " && ";
    }else if (node.op->type() == PLUS && node.first->first_token().type() == STRING_VAL) {
      out << " + ";
    } else {
      out << " " << node.op->lexeme() << " ";
    }
  }

  if(node.rest) {
    node.rest->accept(*this);
  }
  if((node.first && node.op && node.rest) || node.first->first_token().type() == BOOL_VAL) {
    out << ")";
  }
  if(node.negated) {
    out << ")";
  }
}

void Printer::visit(SimpleTerm& node) {
  if(_debug) std::cout << " VISIT SIMPLE TERM " << std::endl; 
  node.rvalue->accept(*this);
}

void Printer::visit(ComplexTerm& node) {
  if(_debug) std::cout << " VISIT COMPLEX TERM " << std::endl;
  node.expr->accept(*this);
}

// rvalues
void Printer::visit(SimpleRValue& node) {
  if(_debug) std::cout << " VISIT SIMPLE R " << std::endl;
  if(node.value.type() == STRING_VAL) {
    out << "\"" << node.value.lexeme() << "\"";
  }else if (node.value.type() == CHAR_VAL) {
    out << "\'" << node.value.lexeme() << "\'";
  }else if (node.value.type() == NIL) {
    out << "nullptr";
  }else {
    out << node.value.lexeme();
  }
}

void Printer::visit(NewRValue& node) {
  out << "new " << node.type_id.lexeme();
}

void Printer::visit(CallExpr& node) {
  if(_debug) std::cout << " VISIT CALL " << std::endl;
  out << node.function_id.lexeme() << "(";
  while(node.arg_list.size() > 1) {
    node.arg_list.front()->accept(*this);
    out << ", ";
    node.arg_list.pop_front();
  }
  if(node.arg_list.size() > 0) {
    node.arg_list.front()->accept(*this);
    node.arg_list.pop_front();
  }
  out << ")";
}

void Printer::visit(IDRValue& node) {
  if(_debug) std::cout << " VISIT IDRVAL " << node.first_token().line() << std::endl;
  auto t = node.path.begin();
  while (t != node.path.end()) {
    out << t->lexeme();
    if(t != --(node.path.end())) {
      out << "->";
    }
    t++;
  }
}

void Printer::visit(NegatedRValue& node) {
   out << "!";
   node.expr->accept(*this);
}


#endif
