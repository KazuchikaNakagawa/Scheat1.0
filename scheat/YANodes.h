//
//  YANodes.hpp
//  scheat
//
//  Created by かずちか on 2020/09/26.
//

#ifndef YANodes_hpp
#define YANodes_hpp

#include <stdio.h>
#include <string>
#include "ScheatBasicStructures.h"
#include "ScheatObjects.h"

namespace scheat {
using namespace std;
using namespace basics;

namespace yaNodes {

class Value {
public:
    TypeData type;
    string value;
    Value(string s, TypeData t) : value(s), type(t) {};
};

class Node {
public:
    SourceLocation location;
    TypeData type;
    virtual Value *codegen(IRStream &) {return nullptr;};
    virtual string userdump() { return "UNDEFINED"; };
};

//--------------------------------------------------------------//

// template class of Statements
class StatementNode : public Node{
public:
    Value * codegen(IRStream &) override{ return nullptr; };
    string userdump() override{ return "UNDEFINED"; };
};

class TermNode : public Node {
public:
    Value * codegen(IRStream &) override{return nullptr;};
    string userdump() override{return "UNDEFINED";};
};

// -------------------------------------------------------------//
// specific classes
class IntTerm : public Node {
public:
    int value;
    IntTerm(Token *t) : value(t->value.intValue) { this->type = TypeData("Int", "i32"); };
    Value * codegen(IRStream &) override;
    string userdump() override{ return to_string(value); };
};

class StringTerm : public Node {
public:
    string value;
    StringTerm(Token *t) : value(t->value.strValue) { this->type = TypeData("String", "%String"); };
    Value * codegen(IRStream &) override;
    string userdump() override{return value;};
};

// -------------------------------------------------------------//

// term : TermNode
//      | TermNode OP term
//      | OP term
//      | term OP
class Term : public Node{
public:
    unique_ptr<TermNode> lhs = nullptr;
    Operator *op = nullptr;
    unique_ptr<Term> rhs = nullptr;
    Value * codegen(IRStream &) override;
    string userdump() override;
    __deprecated_msg("this class is for unique_ptr")
    Term() {};
    static unique_ptr<Term> init(unique_ptr<TermNode>);
    static unique_ptr<Term> initAsOperatedExpr(unique_ptr<TermNode>,
                                               Operator *,
                                               unique_ptr<Term>);
    static unique_ptr<Term> initAsPrefixOperatorExpr(Operator *,
                                                     unique_ptr<Term>);
    static unique_ptr<Term> initAsPostfixOperatorExpr(unique_ptr<Term>,
                                                      Operator *);
    
};

// primary : term
//         | term OP primary
//         | OP primary
//         | primary OP
class PrimaryExpr : public Node {
public:
    unique_ptr<Term> lhs;
    Operator *op;
    unique_ptr<PrimaryExpr> rhs;
    Value * codegen(IRStream &) override;
    string userdump() override;
    __deprecated_msg("this class is for unique_ptr")
    PrimaryExpr() {};
    static unique_ptr<PrimaryExpr> init(unique_ptr<Term>);
    static unique_ptr<PrimaryExpr> initAsOperatedExpr(unique_ptr<Term>,
                                               Operator *,
                                               unique_ptr<PrimaryExpr>);
    static unique_ptr<PrimaryExpr> initAsPrefixOperatorExpr(Operator *,
                                                     unique_ptr<PrimaryExpr>);
    static unique_ptr<PrimaryExpr> initAsPostfixOperatorExpr(unique_ptr<PrimaryExpr>,
                                                      Operator *);
};

// expr : primary
//      | primary OP expr
//      | OP expr
//      | expr OP
class Expr : public Node {
public:
    unique_ptr<PrimaryExpr> lhs;
    Operator *op;
    unique_ptr<Expr> rhs;
    Value * codegen(IRStream &) override;
    string userdump() override;
    __deprecated_msg("this class is for unique_ptr")
    Expr() {};
    static unique_ptr<Expr> init(unique_ptr<PrimaryExpr>);
    static unique_ptr<Expr> initAsOperatedExpr(unique_ptr<PrimaryExpr>,
                                                      Operator *,
                                                      unique_ptr<Expr>);
    static unique_ptr<Expr> initAsPrefixOperatorExpr(Operator *,
                                                            unique_ptr<Expr>);
    static unique_ptr<Expr> initAsPostfixOperatorExpr(unique_ptr<Expr>,
                                                      Operator *);
};

// statement : StatementNode .
//           | StatementNode , statement
class Statement : public Node {
public:
    unique_ptr<StatementNode> statement;
    Token *perTok;
    unique_ptr<Statement> stmt;
    Value * codegen(IRStream &) override;
    string userdump() override;
};

// statements : statement
//            | statement statements
class Statements : public Node {
public:
    unique_ptr<Statement> statement;
    unique_ptr<Statements> statements;
    Value * codegen(IRStream &) override;
    string userdump() override;
    
};

}

}

#endif /* YANodes_hpp */
