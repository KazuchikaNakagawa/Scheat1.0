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
    virtual ~Node() {};
};

class Expr;

//--------------------------------------------------------------//

// template class of Statements
class StatementNode : public Node{
public:
    Value * codegen(IRStream &) override{ return nullptr; };
    string userdump() override{ return "UNDEFINED"; };
    virtual ~StatementNode() {};
};

class TermNode : public Node {
public:
    Value * codegen(IRStream &) override{return nullptr;};
    string userdump() override{return "UNDEFINED";};
    virtual ~TermNode() {};
};

// -------------------------------------------------------------//
// specific classes
class IntTerm : public TermNode {
public:
    int value;
    IntTerm(Token *t) : value(t->value.intValue) { this->type = TypeData("Int", "i32");
        location = t->location;
    };
    Value * codegen(IRStream &) override;
    string userdump() override{ return to_string(value); };
};

class StringTerm : public TermNode {
public:
    string value;
    StringTerm(Token *t) : value(t->value.strValue) { this->type = TypeData("the Character", "i8*");
        location = t->location;
    };
    Value * codegen(IRStream &) override;
    string userdump() override{return value;};
};

class BoolTerm : public TermNode {
public:
    bool value;
    BoolTerm(Token *t) : value(t->value.boolValue) {
        type = TypeData("Bool", "i1");
    };
    Value * codegen(IRStream &) override;
    string userdump() override{
        if (value) {
            return "true";
        }else{
            return "false";
        }
    };
};

class FloatTerm : public TermNode {
public:
    double value;
    FloatTerm(Token *t) : value(t->value.doubleValue) {
        type = TypeData("Float", "double");
    };
    Value * codegen(IRStream &) override;
    string userdump() override{
        return to_string(value);
    };
};

class IdentifierTerm : public TermNode {
public:
    string value;
    vector<unique_ptr<Expr>> args;
    IdentifierTerm(Token *t, TypeData ty){
        type = ty;
        value = t->value.doubleValue;
    }
    bool isFunc = false;
    void addArg(unique_ptr<Expr>);
    Value * codegen(IRStream &) override;
    string userdump() override;
};

// idexpr : idterm
//        | idexpr . idterm
class IdentifierExpr : public TermNode {
public:
    unique_ptr<IdentifierExpr> lhs;
    Token *perTok;
    unique_ptr<IdentifierTerm> rhs;
    IdentifierExpr(unique_ptr<IdentifierTerm>);
    IdentifierExpr(unique_ptr<IdentifierExpr>, Token *, unique_ptr<IdentifierTerm>);
    Value * codegen(IRStream &) override;
    string userdump() override{
        if (lhs != nullptr && perTok != nullptr) {
            return lhs->userdump() + perTok->value.strValue + rhs->userdump();
        }
        return rhs->userdump();
    };
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
    string userdump() override{return "UNDEFINED";};
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
