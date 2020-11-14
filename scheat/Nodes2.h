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

namespace nodes2 {

class Value {
public:
    TypeData type;
    string value;
    Value(string s, TypeData t) : value(s), type(t) {};
    string asValue(){
        return type.ir_used + " " + value;
    };
};

class Node {
public:
    SourceLocation location;
    TypeData type;
    virtual Value *codegen(IRStream &) {return nullptr;};
    virtual Value *codegenAsRef(IRStream &f) {return codegen(f);};
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
    virtual void commitment() {};
};

class Expr : public Node {
public:
    Value * codegen(IRStream &) override{ return nullptr; };
    string userdump() override {return "UNDEFINED";};
    
};

class PrimaryExpr : public Expr {
public:
    Value * codegen(IRStream &) override{ return nullptr; };
    string userdump() override{ return "UNDEFINED"; };
};

class Term : public PrimaryExpr {
public:
    Value * codegen(IRStream &) override{return nullptr;};
    string userdump() override{return "UNDEFINED";};
    virtual ~Term() {};
};



// -------------------------------------------------------------//
// specific classes
class IntTerm : public Term {
public:
    int value;
    IntTerm(Token *t) : value(t->value.intValue) { this->type = TypeData("Int", "i32");
        location = t->location;
    };
    Value * codegen(IRStream &) override;
    string userdump() override{ return to_string(value); };
};

class StringTerm : public Term {
public:
    // to do
    /*
     by using bitcast, (i8* @.str to %String*) we can generate String
     object directly.
     @.str = unnamed_addr inbound [7 x i8] "Hello"
     @s = bitcast (i8* getelementptr ()() to %String*)
     */
    string value;
    StringTerm(Token *t) : value(t->value.strValue) { this->type = TypeData("String", "%String");
        location = t->location;
    };
    Value * codegen(IRStream &) override;
    string userdump() override{return value;};
};

class BoolTerm : public Term {
public:
    bool value;
    BoolTerm(Token *t) : value(t->value.boolValue) {
        type = TypeData("Bool", "i1");
        location = t->location;
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

class FloatTerm : public Term {
public:
    double value;
    FloatTerm(Token *t) : value(t->value.doubleValue) {
        type = TypeData("Float", "double");
        location = t->location;
    };
    Value * codegen(IRStream &) override;
    string userdump() override{
        return to_string(value);
    };
};

//class IdentifierExprTemplate : public Term {
//public:
//    Value * codegen(IRStream &) override{ return nullptr; };
//    string userdump() override { return "UNDEFINED"; };
//};
class IdentifierExpr : public Term {
public:
    Value * codegen(IRStream &) override{ return nullptr; };
    virtual Value * codegenAsRef() { return nullptr; };
    string userdump() override{ return "UNDEFINED"; };
};

class IdentifierTermTemplate : public IdentifierExpr {
public:
    string value;
    Value * codegen(IRStream &) override { return nullptr; };
    string userdump() override { return "UNDEFINED"; };
    virtual void addArgument(bool,unique_ptr<Expr>) {};
    Value * codegenAsRef(IRStream &) override{ return nullptr; };
};

class GlobalExpr;

class IdentifierTerm;

class AccessIdentifierExpr : public IdentifierExpr {
public:
    unique_ptr<IdentifierExpr> parent;
    unique_ptr<IdentifierTerm> child;
    static unique_ptr<AccessIdentifierExpr>
    init(unique_ptr<IdentifierExpr> p, unique_ptr<IdentifierTerm> c = nullptr);
    Value * codegen(IRStream &) override;
    Value * codegenAsRef(IRStream &) override;
    string userdump() override;
};

class TopIdentifierExpr : public Term {
public:
    Value * codegen(IRStream &) override { return nullptr; };
    string userdump() override{ return ""; };
    
};

class GlobalExpr : public IdentifierExpr {
public:
    unique_ptr<TopIdentifierExpr> ptr;
    Value * codegen(IRStream &) override;
    string userdump() override{ return "UNDEFINED"; };
    static unique_ptr<GlobalExpr> init(unique_ptr<TopIdentifierExpr> ptr);
    Value * codegenAsRef(IRStream &) override;
};

class VariableAttributeExpr {
public:
    SourceLocation location;
    TypeData type;
    Variable *varptr;
    string userdump(){return varptr->mangledName;};
    Value * codegenWithParent(Value *, IRStream &);
    static unique_ptr<VariableAttributeExpr>
    init(Variable *, SourceLocation location);
};

class FunctionAttributeExpr {
public:
    SourceLocation location;
    TypeData type;
    Function *func;
    vector<Value *> values = {};
    string userdump() {return func->name;};
    Value * codegenWithParent(Value *, IRStream &);
    static unique_ptr<FunctionAttributeExpr>
    init(Function *, SourceLocation);
};

class IdentifierTerm : public IdentifierExpr {
public:
    unique_ptr<VariableAttributeExpr> varptr = nullptr;
    unique_ptr<FunctionAttributeExpr> funcptr = nullptr;
    string userdump() override{
        if (varptr == nullptr) {
            return funcptr->userdump();
        }else{
            return varptr->userdump();
        }
    }
    Value * codegenWith(Value *,IRStream &);
    Value * codegen(IRStream &) override;
    static unique_ptr<IdentifierTerm>
    init(unique_ptr<VariableAttributeExpr> v){
        auto ptr = make_unique<IdentifierTerm>();
        ptr->location = v->location;
        ptr->type = v->type;
        ptr->varptr = move(v);
        return ptr;
    };
    static unique_ptr<IdentifierTerm>
    init(unique_ptr<FunctionAttributeExpr> v){
        auto ptr = make_unique<IdentifierTerm>();
        ptr->location = v->location;
        ptr->type = v->type;
        ptr->funcptr = move(v);
        return ptr;
    }
};

class VariableTerm : public TopIdentifierExpr {
public:
    string value;
    Value * codegen(IRStream &) override{
        return nullptr;
    };
    string userdump() override;
    //void addArgument(bool, unique_ptr<Expr>) override{};
    static unique_ptr<VariableTerm> init(Token *, TypeData);
};

class FunctionCallTerm : public TopIdentifierExpr {
public:
    Function *func;
    string value;
    vector<unique_ptr<Expr>> args {};
    Value * codegen(IRStream &) override;
    string userdump() override;
    void addArgument(bool insertToTop, unique_ptr<Expr> arg) {
        if (insertToTop) {
            args.insert(args.begin(), move(arg));
        }else{
            args.push_back(move(arg));
        }
    };
    static unique_ptr<FunctionCallTerm> init(Token *, Function *);
};

//class AccessIdentifierTerm : public IdentifierTermTemplate {
//public:
//    Value * codegen(IRStream &) override;
//    string userdump() override{ return "UNDEFINED"; };
//    unique_ptr<Expr> lhs;
//    unique_ptr<IdentifierTermTemplate> rhs;
//    int index;
//    static unique_ptr<AccessIdentifierTerm> init(unique_ptr<Expr>,
//                                                 unique_ptr<IdentifierTermTemplate>,
//                                                 int);
//    void addArgument(bool, unique_ptr<Expr>) override{};
//    ~AccessIdentifierTerm() {};
//};
//
//class UnknownIdentifierTerm : public IdentifierTermTemplate {
//public:
//    static unique_ptr<UnknownIdentifierTerm> init(Token *tok){
//        auto ptr = make_unique<UnknownIdentifierTerm>();
//        ptr->value = tok->value.strValue;
//        ptr->type = TypeData("COMPILED-NEW-ID", "i8*");
//        return ptr;
//    }
//    Value * codegen(IRStream &) override{
//        return new Value(value, type);
//    }
//
//    string userdump() override{
//        return "Variable(" + value + ")";
//    }
//
//};

//class TheIdentifierTerm : public IdentifierTermTemplate {
//public:
//    unique_ptr<IdentifierExprTemplate> id;
//    static unique_ptr<TheIdentifierTerm> init(unique_ptr<IdentifierExprTemplate>);
//    Value * codegen(IRStream &) override;
//    string userdump() override{
//        return id->userdump() + ".pointer";
//    };
//};
//
//class NewIdentifierExpr : public IdentifierExprTemplate {
//public:
//    Value * codegen(IRStream &) override;
//    unique_ptr<IdentifierTermTemplate> id;
//    static unique_ptr<NewIdentifierExpr> init(unique_ptr<IdentifierTermTemplate> ident, TypeData type){
//        auto ptr = make_unique<NewIdentifierExpr>();
//        ptr->id = move(ident);
//        ptr->type = type;
//        return ptr;
//    };
//};

// -------------------------------------------------------------//

class ParenthesesExpr : public Term {
public:
    unique_ptr<Expr> expr;
    Value * codegen(IRStream &f) override{
        return expr->codegen(f);
    };
    string userdump() override{
        return expr->userdump();
    };
    static unique_ptr<ParenthesesExpr> init(unique_ptr<Expr> node){
        auto ptr = make_unique<ParenthesesExpr>();
        ptr->type = node->type;
        ptr->expr = move(node);
        return ptr;
    }
};

class InfixOperatorTerm : public Term {
public:
    unique_ptr<Term> lhs;
    Operator *op;
    unique_ptr<Term> rhs;
    Value * codegen(IRStream &) override;
    string userdump() override;
    InfixOperatorTerm(){};
    static unique_ptr<InfixOperatorTerm> init(unique_ptr<Term>,
                                              Operator *,
                                              unique_ptr<Term>);
    
};

class PostfixOperatorTerm : public Term {
public:
    unique_ptr<Term> lhs;
    Operator *op;
    Value * codegen(IRStream &) override;
    string userdump() override;
    PostfixOperatorTerm() {};
    static unique_ptr<PostfixOperatorTerm> init(unique_ptr<Term> t,
                                                Operator *op){
        auto ptr = make_unique<PostfixOperatorTerm>();
        ptr->type = op->return_type;
        ptr->location = t->location;
        ptr->lhs = move(t);
        ptr->op = op;
        return ptr;
    };
};

class PrefixOperatorTerm : public Term {
public:
    Operator *op;
    unique_ptr<Term> rhs;
    Value * codegen(IRStream &) override;
    string userdump() override;
    PrefixOperatorTerm() {};
    static unique_ptr<PrefixOperatorTerm> init(Operator *,
                                               unique_ptr<Term>);
};

class InfixOperatorPrimaryExpr : public PrimaryExpr {
public:
    unique_ptr<Term> lhs;
    Operator *op;
    unique_ptr<PrimaryExpr> rhs;
    __deprecated
    InfixOperatorPrimaryExpr() {};
    static unique_ptr<InfixOperatorPrimaryExpr> init(unique_ptr<Term>,
                                                     Operator *,
                                                     unique_ptr<PrimaryExpr>);
    Value * codegen(IRStream &) override;
    string userdump() override{
        return lhs->userdump() + op->value + rhs->userdump();
    }
};

class PrefixOperatorPrimaryExpr : public PrimaryExpr {
public:
    Operator *op;
    unique_ptr<PrimaryExpr> rhs;
    __deprecated
    PrefixOperatorPrimaryExpr() {};
    static unique_ptr<PrefixOperatorPrimaryExpr> init(Operator *,
                                                     unique_ptr<PrimaryExpr>);
    Value * codegen(IRStream &) override;
    string userdump() override{ return op->value + rhs->userdump(); };
};

class PostfixOperatorPrimaryExpr : public PrimaryExpr {
public:
    unique_ptr<PrimaryExpr> lhs;
    Operator *op;
    __deprecated
    PostfixOperatorPrimaryExpr() {};
    static unique_ptr<PostfixOperatorPrimaryExpr> init(unique_ptr<PrimaryExpr>,
                                                     Operator *);
    Value * codegen(IRStream &) override;
    string userdump() override{ return lhs->userdump() + op->value; };
};

// primary : term
//         | term OP primary
//         | OP primary
//         | primary OP
//class OperatedPrimaryExpr : public PrimaryExpr {
//public:
//    bool syntaxedExpr = false;
//    unique_ptr<Term> lhs;
//    Operator *op;
//    unique_ptr<PrimaryExpr> rhs;
//    Value * codegen(IRStream &) override;
//    unique_ptr<PrimaryExpr> syntaxNode = nullptr;
//    string userdump() override;
//    __deprecated_msg("this class is for unique_ptr")
//    OperatedPrimaryExpr() {};
//    static __deprecated unique_ptr<OperatedPrimaryExpr> init(unique_ptr<Term>);
//    static unique_ptr<OperatedPrimaryExpr> initAsOperatedExpr(unique_ptr<Term>,
//                                               Operator *,
//                                               unique_ptr<OperatedPrimaryExpr>);
//    static unique_ptr<OperatedPrimaryExpr> initAsPrefixOperatorExpr(Operator *,
//                                                     unique_ptr<OperatedPrimaryExpr>);
//    static unique_ptr<OperatedPrimaryExpr> initAsPostfixOperatorExpr(unique_ptr<OperatedPrimaryExpr>,
//                                                      Operator *);
//    static unique_ptr<OperatedPrimaryExpr> initAsSyntaxExpr(unique_ptr<PrimaryExpr>);
//};

class InfixOperatorExpr : public Expr {
public:
    unique_ptr<PrimaryExpr> lhs;
    Operator *op;
    unique_ptr<Expr> rhs;
    Value * codegen(IRStream &) override;
    string userdump() override;
    static unique_ptr<InfixOperatorExpr> init(unique_ptr<PrimaryExpr> p,
                                              Operator *op,
                                              unique_ptr<Expr> e){
        auto ptr = make_unique<InfixOperatorExpr>();
        ptr->location = p->location;
        ptr->op = op;
        ptr->lhs = move(p);
        ptr->rhs = move(e);
        ptr->type = op->return_type;
        return ptr;
    };
};

class PrefixOperatorExpr : public Expr {
public:
    Operator *op;
    unique_ptr<Expr> rhs;
    Value * codegen(IRStream &) override;
    string userdump() override;
    static unique_ptr<PrefixOperatorExpr> init(Operator *,
                                               unique_ptr<Expr>);
};

class PostfixOperatorExpr : public Expr {
public:
    unique_ptr<Expr> lhs;
    Operator *op;
    Value * codegen(IRStream &) override;
    string userdump() override;
    static unique_ptr<PostfixOperatorExpr> init(unique_ptr<Expr> e,
                                                Operator *op){
        auto ptr = make_unique<PostfixOperatorExpr>();
        ptr->type = op->return_type;
        ptr->location = e->location;
        ptr->lhs = move(e);
        ptr->op = op;
        return ptr;
    };
};

// expr : primary
//      | primary OP expr
//      | OP expr
//      | expr OP
//      | expr_syntax
//class OperatedExpr : public Node {
//public:
//    bool syntaxedExpr = false;
//    unique_ptr<PrimaryExpr> lhs;
//    Operator *op;
//    unique_ptr<Expr> rhs;
//    unique_ptr<Expr> syntax = nullptr;
//    Value * codegen(IRStream &) override;
//    string userdump() override{return "UNDEFINED";};
//    __deprecated_msg("this class is for unique_ptr")
//    OperatedExpr() {};
//    static unique_ptr<OperatedExpr> init(unique_ptr<PrimaryExpr>);
//    static unique_ptr<OperatedExpr> initAsOperatedExpr(unique_ptr<PrimaryExpr>,
//                                                      Operator *,
//                                                      unique_ptr<Expr>);
//    static unique_ptr<OperatedExpr> initAsPrefixOperatorExpr(Operator *,
//                                                            unique_ptr<Expr>);
//    static unique_ptr<OperatedExpr> initAsPostfixOperatorExpr(unique_ptr<Expr>,
//                                                      Operator *);
//    static unique_ptr<OperatedExpr> initAsSyntaxedExpr(unique_ptr<Expr>);
//};

// statement : StatementNode .
//           | StatementNode , statement
class Statement : public Node {
public:
    unique_ptr<StatementNode> statement;
    Token *perTok;
    unique_ptr<Statement> stmt;
    Value * codegen(IRStream &) override { return nullptr; };
    string userdump() override{ return "UNDEFINED"; };
    Statement() {};
    Statement(unique_ptr<Statement> stn){
        stmt = move(stn);
    }
};

// statements : statement
//            | statements statement
class Statements : public Node {
public:
    unique_ptr<Statements> statements;
    unique_ptr<Statement> statement;
    Value * codegen(IRStream &) override;
    string userdump() override{ return "UNDEFINED"; };
    Statements() {
        statement = nullptr;
        statements = nullptr;
    };
    
    Statements(unique_ptr<Statements> stmts, unique_ptr<Statement> stmt){
        statements = move(stmts);
        statement = move(stmt);
    };
};


// ----------------------------------------------------------//

// this ID is expr.
// expr : expr of IDExpr
// this statement is expected in global and local function
class DeclareVariableStatement : public StatementNode {
public:
    string name;
    
    bool isPublic;
    bool isConstant;
    bool isNullable;
    
    unique_ptr<Expr> value;
    static unique_ptr<DeclareVariableStatement>
    init(Token *, unique_ptr<Expr>,bool pub = true, bool con = false, bool nul = false);
    Value * codegen(IRStream &) override;
};

class IfStatement : public StatementNode {
public:
    unique_ptr<Expr> condition;
    unique_ptr<Statement> thenS;
    unique_ptr<Statement> elseS;
    // todo
    string userdump() override{
        return "if (" + condition->userdump() + "){\n" + thenS->userdump() + "\n}else{\n" + elseS->userdump() + "\n}";
    }
    
    Value * codegen(IRStream &) override;
};

// ====================================
class DataHolder {
public:
    unique_ptr<Statements> statements = nullptr;
};

}

}

#endif /* YANodes_hpp */
