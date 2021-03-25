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
#include "ScheatTypes.h"
#include "ScheatToken.h"
#include "ScheatContextStructures.h"
#include "ScheatContext.h"

namespace scheat {
using namespace std;
struct SourceLocation;
namespace nodes2 {
class Value {
public:
    TypeData type;
    string value;
    Value(string s, TypeData t) : value(s), type(t) {};
    string asValue(){
        return type.ir_used + " " + value;
    };
    static Value VoidValue;
};

class Node {
public:
    SourceLocation location;
    TypeData type = TypeData::VoidType;
    Context *context;
    virtual Value *codegen(IRStream &f) {return nullptr;};
    virtual Value *codegenAsRef(IRStream &f) {return codegen(f);};
    virtual string userdump() { return "UNDEFINED"; };
    Node();
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
    virtual Value * codegenAsRef(IRStream &) override{ return nullptr; };
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

class TheIdentifierExpr : public IdentifierExpr {
public:
    unique_ptr<IdentifierExpr> expr;
    static unique_ptr<TheIdentifierExpr> init(Token *, unique_ptr<IdentifierExpr>);
    string userdump() override{ return "&" + expr->userdump(); };
    Value * codegen(IRStream &) override;
    Value * codegenAsRef(IRStream &) override;
};

class TopIdentifierExpr : public IdentifierExpr {
public:
    Value * codegen(IRStream &) override { return nullptr; };
    string userdump() override{ return ""; };
    Value * codegenAsRef(IRStream &) override{ return nullptr; };
};

class GlobalExpr : public IdentifierExpr {
public:
    unique_ptr<TopIdentifierExpr> ptr;
    Value * codegen(IRStream &) override;
    string userdump() override{ return "UNDEFINED"; };
    static unique_ptr<GlobalExpr> init(unique_ptr<TopIdentifierExpr> ptr);
    Value * codegenAsRef(IRStream &) override;
};

class IdentifierTerm : public IdentifierExpr {
public:
    string userdump() override{
        return "UNDEFINED";
    }
    virtual Value * codegenWithParent(Value *,IRStream &) {return nullptr;};
    Value * codegen(IRStream &) override;
    Value * codegenAsRef(IRStream &) override{ return nullptr; };
};

class VariableAttributeExpr : public IdentifierTerm {
public:
    SourceLocation location;
    
    Property varindex;
    string userdump()override{return "."+to_string(varindex.index);};
    Value * codegenWithParent(Value *, IRStream &)override;
    Value * codegenAsRef(IRStream &) override;
    static unique_ptr<VariableAttributeExpr>
    init(Property, SourceLocation location);
    VariableAttributeExpr() { varindex = Property(); };
};

class ArgumentExpr;

class FunctionAttributeExpr : public IdentifierTerm {
public:
    SourceLocation location;
    TypeData type;
    Function *func;
    unique_ptr<ArgumentExpr> args = nullptr;
    vector<Value *> values = {};
    string userdump() override{return func->name;};
    Value * codegenWithParent(Value *, IRStream &) override;
    Value * codegen(IRStream &) override;
    Value * codegenAsRef(IRStream &) override;
    static unique_ptr<FunctionAttributeExpr>
    init(Function *, unique_ptr<ArgumentExpr>, SourceLocation);
};

class NewIdentifierExpr : public IdentifierExpr {
public:
    string value;
    bool isGlobal = false;
    bool isPublic = true;
    
    // if this is not restricted, this become nullptr.
    // it must not be null till code-generating.
    TypeData *type;
    Value * codegenAsRef(IRStream &) override;
    Value * codegen(IRStream &f) override{ return codegenAsRef(f);};
    static unique_ptr<NewIdentifierExpr>
    init(SourceLocation, string, TypeData *);
    void setType(TypeData);
};

class VariableTerm : public TopIdentifierExpr {
public:
    string value;
    Value * codegen(IRStream &) override;
    string userdump() override;
    //void addArgument(bool, unique_ptr<Expr>) override{};
    static unique_ptr<VariableTerm> init(Token *, TypeData);
    Value * codegenAsRef(IRStream &) override;
};

class FunctionCallTerm : public TopIdentifierExpr {
public:
    Function *func;
    string value;
    
    unique_ptr<ArgumentExpr> args;
    Value * codegen(IRStream &) override;
    string userdump() override;
//    void addArgument(bool insertToTop, unique_ptr<ArgumentExpr> arg) {
//        if (insertToTop) {
//            args.insert(args.begin(), move(arg));
//        }else{
//            args.push_back(move(arg));
//        }
//    };
    static unique_ptr<FunctionCallTerm> init(Token *, Function *, unique_ptr<ArgumentExpr>);
};


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

class ArgumentExpr : public Expr {
public:
    unique_ptr<ArgumentExpr> container = nullptr;
    unique_ptr<Expr> self;
    Value * codegen(IRStream &) override{ return nullptr; };
    string userdump() override{
        if (container == nullptr) return self->userdump();
        return container->userdump() + ", " + self->userdump();
    };
    vector<Value *> codegenAsArray(IRStream &);
    vector<TypeData> getTypes();
    static unique_ptr<ArgumentExpr> init(unique_ptr<Expr>);
    static unique_ptr<ArgumentExpr> addArg(unique_ptr<ArgumentExpr>, unique_ptr<Expr>);
    string demangled(){
        string buf = "_";
        auto arr = getTypes();
        for (auto t : arr) {
            buf += t.name + "_";
        }
        return buf;
    }
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

class CastExpr : public Expr {
public:
    unique_ptr<Expr> expr;
    Value * codegen(IRStream &) override;
    string userdump() override;
    static unique_ptr<CastExpr> init(TypeData, unique_ptr<Expr>);
};

class LoadExpr : public IdentifierExpr {
public:
    unique_ptr<Expr> expr;
    Value * codegen(IRStream &) override;
    string userdump() override;
    static unique_ptr<LoadExpr> init(TypeData tp, unique_ptr<Expr> ptr){
        auto l = make_unique<LoadExpr>();
        l->location = ptr->location;
        l->expr = move(ptr);
        l->type = tp;
        return l;
    };
};

class AllocationExpr : public Expr {
public:
    unique_ptr<Expr> expr;
    Value *codegen(IRStream &) override;
    string userdump() override;
    static unique_ptr<AllocationExpr> init(unique_ptr<Expr>);
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
    Value * codegen(IRStream &f) override;
    string userdump() override{ return "UNDEFINED"; };
    Statement() {};
    Statement(unique_ptr<Statement> stn){
        stmt = move(stn);
    }
    static unique_ptr<Statement> init(unique_ptr<Statement> s,
                                      unique_ptr<StatementNode> n){
        auto ptr = make_unique<Statement>();
        ptr->statement = move(n);
        ptr->stmt = move(s);
        return ptr;
    };
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
    
    bool isGlobal = false;
    
    bool isPublic;
    bool isConstant;
    bool isNullable;
    
    unique_ptr<Expr> value;
    static unique_ptr<DeclareVariableStatement>
    init(unique_ptr<NewIdentifierExpr>, unique_ptr<Expr>,bool pub = true, bool con = false, bool nul = false);
    Value * codegen(IRStream &) override;
};

class DeclareFunctionStatement : public StatementNode {
public:
    Function* name;
    vector<TypeData> argTypes;
    vector<string> argNames;
    Context *Fcontext;
    unique_ptr<Statement> body;
    Value * codegen(IRStream &) override;
    static unique_ptr<DeclareFunctionStatement> init(Function *func, unique_ptr<Statement> s, Context *context){
        auto ptr = make_unique<DeclareFunctionStatement>();
        ptr->name = func;
        ptr->body = move(s);
        ptr->Fcontext = context;
        ptr->location = ptr->body->location;
        return ptr;
    };
};

class FunctionCallStatement : public StatementNode {
public:
    unique_ptr<Expr> callee;
    Value * codegen(IRStream &f) override{
        return callee->codegen(f);
    }
    string userdump() override{
        return "call " + callee->userdump();
    }
    static unique_ptr<FunctionCallStatement>
    init(unique_ptr<Expr> p){
        auto ptr = make_unique<FunctionCallStatement>();
        ptr->callee = move(p);
        return ptr;
    }
};

class DoneStatement : public StatementNode {
public:
    Value * codegen(IRStream &) override{ return nullptr; };
    string userdump() override { return ""; };
};

class ReturnStatement : public StatementNode {
public:
    unique_ptr<Expr> expr;
    Value * codegen(IRStream &f) override{
        auto v = expr->codegen(f);
        f << "ret " << v->asValue() << "\n";
        return nullptr;
    };
    string userdump() override{
        return "return " + expr->userdump();
    };
    static unique_ptr<ReturnStatement>
    init(unique_ptr<Expr> p){
        auto ptr = make_unique<ReturnStatement>();
        ptr->type = p->type;
        ptr->location = p->location;
        ptr->expr = move(p);
        return ptr;
    }
};

class BreakStatement : public StatementNode {
public:
    string scope;
    Value * codegen(IRStream &) override;
    string userdump() override{ return "break;"; };
    static unique_ptr<BreakStatement>
    init(SourceLocation loc, string sc){
        auto ptr = make_unique<BreakStatement>();
        ptr->location = loc;
        ptr->scope = sc;
        ptr->type = TypeData::VoidType;
        return ptr;
    }
};

class ContinueStatement : public StatementNode {
public:
    string scope;
    Value * codegen(IRStream &) override;
    string userdump() override{ return "break;"; };
    static unique_ptr<ContinueStatement>
    init(SourceLocation loc, string sc){
        auto ptr = make_unique<ContinueStatement>();
        ptr->location = loc;
        ptr->scope = sc;
        ptr->type = TypeData::VoidType;
        return ptr;
    }
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
    static unique_ptr<IfStatement>
    init(unique_ptr<Expr> b, unique_ptr<Statement> t, unique_ptr<Statement> e){
        auto ptr = make_unique<IfStatement>();
        ptr->location = b->location;
        ptr->condition = move(b);
        ptr->thenS = move(t);
        ptr->elseS = move(e);
        return ptr;
    }
};

class ForStatement : public StatementNode {
public:
    unique_ptr<Expr> count;
    unique_ptr<Statement> body;
    string scopeName;
    string userdump() override{
        return "for (" + count->userdump() + "):" + body->userdump();
    };
    Value * codegen(IRStream &) override;
    static unique_ptr<ForStatement>
    init(unique_ptr<Expr> intExpr, unique_ptr<Statement> statement, string sn){
        auto ptr = make_unique<ForStatement>();
        ptr->location = intExpr->location;
        ptr->count = move(intExpr);
        ptr->body = move(statement);
        ptr->scopeName = sn;
        return ptr;
    };
};

class ClassStatement : public StatementNode {
public:
    Value * codegen(IRStream &) override{
        return nullptr;
    };
    string userdump() override{
        return "";
    };
};

class ClassStatements : public ClassStatement {
public:
    unique_ptr<ClassStatements> statements;
    unique_ptr<ClassStatement> statement;
    static unique_ptr<ClassStatements>
    init(unique_ptr<ClassStatements> ptr1, unique_ptr<ClassStatement> ptr2){
        auto ptr = make_unique<ClassStatements>();
        ptr->statements = move(ptr1);
        ptr->statement = move(ptr2);
        return ptr;
    }
    static unique_ptr<ClassStatements>
    init(unique_ptr<ClassStatement> ptr1){
        auto ptr = make_unique<ClassStatements>();
        ptr->statements = nullptr;
        ptr->statement = move(ptr1);
        return ptr;
    }
    Value * codegen(IRStream &f) override{
        if (statements) {
            statements->codegen(f);
        }
        statement->codegen(f);
        return nullptr;
    }
};

class ClassDefinitionStatement : public StatementNode {
public:
    
    // Name class:
    //     (declarations).
    // end.
    string name;
    
    // saving class object
    Class *classObject;
    
    // context of initializer function
    Context *initializer;
    
    // context of deinitializer function
    Context *deinitializer;
    
    // context of copy function
    Context *copyinitializer;
    
    Value * codegen(IRStream &) override;
    string userdump() override{
        return "declare class." +  name + ":\n";
    };
    
    Value *getProperty(Context *,IRStream &, string);
    
    // (declarations)
    unique_ptr<ClassStatements> statements;
    static unique_ptr<ClassDefinitionStatement>
    init(string _namespace,Token *idtok,unique_ptr<ClassStatements> stmt){
        auto ptr = make_unique<ClassDefinitionStatement>();
        ptr->statements = move(stmt);
        ptr->name = _namespace + "_" + idtok->value.strValue;
        ptr->location = idtok->location;
        return ptr;
    }
};

class WhileStatement : public StatementNode {
public:
    
    // while (condition), (statements).
    
    // (condition)
    unique_ptr<Expr> condition;
    
    // (statements)
    unique_ptr<Statement> body;
    
    // compiler needs to memorize
    string scopeName;
    
    string userdump() override{
        return "while (" + condition->userdump() + ") {" + body->userdump() + "}";
    }
    Value * codegen(IRStream &) override;
    static
    unique_ptr<WhileStatement> init(unique_ptr<Expr> b,
                                    unique_ptr<Statement> s,
                                    string sn){
        auto ptr = make_unique<WhileStatement>();
        ptr->location = b->location;
        ptr->scopeName = sn;
        ptr->condition = move(b);
        ptr->body = move(s);
        return ptr;
    }
};

class ReassignStatement : public StatementNode {
public:
    
    // (pointer) is (value)
    
    // (pointer)
    unique_ptr<IdentifierExpr> variable;
    
    // (value)
    unique_ptr<Expr> value;
    
    static unique_ptr<ReassignStatement>
    init(unique_ptr<IdentifierExpr>, unique_ptr<Expr>);
    
    Value * codegen(IRStream &) override;
    
    string userdump() override{
        return "assign(" + variable->userdump() + ", " + value->userdump() + ")";
    }
};

class PrintStatement : public StatementNode {
public:
    
    // print(Arg)
    
    // Arg
    unique_ptr<ArgumentExpr> arg;
    
    static unique_ptr<PrintStatement> init(unique_ptr<ArgumentExpr> a){
        auto ptr = make_unique<PrintStatement>();
        ptr->location = a->location;
        ptr->arg = move(a);
        return ptr;
    };
    Value * codegen(IRStream &) override;
    string userdump() override{ return "print(" + arg->userdump() + ")"; };
};


class PropertyDeclareStatement : public ClassStatement {
public:
    
    // its A is V (of Type).
    
    Value * codegen(IRStream &) override;
    
    string userdump() override{
        return "property : " + name + "\n";
    };
    
    // Type
    TypeData *propertyType;
    
    // A
    string name;

    // V
    unique_ptr<Expr> initializedValue = nullptr;
    
    Class *host;
    
    Property *p;
    
    static unique_ptr<PropertyDeclareStatement>
    init(Class *c, string name, unique_ptr<Expr> expr, Property *p,TypeData *t = nullptr){
        auto ptr = make_unique<PropertyDeclareStatement>();
        ptr->host = c;
        ptr->name = name;
        if (!t) {
            ptr->propertyType = new TypeData(expr->type);
        }else{
            ptr->propertyType = t;
        }
        ptr->initializedValue = move(expr);
        ptr->p = p;
        return ptr;
    }
};

class MethodDeclareStatement : public ClassStatement {
public:
    Function *func;
    Class *host;
    Value * codegen(IRStream &) override;
    string userdump() override{
        return "UNDEFINED";
    }
    unique_ptr<Statement> body;
    static unique_ptr<MethodDeclareStatement>
    init(Class *c,Function *f, unique_ptr<Statement> statements){
        auto ptr = make_unique<MethodDeclareStatement>();
        ptr->host = c;
        ptr->body = move(statements);
        ptr->context = f->context;
        ptr->func = f;
        return ptr;
    }
    
};

// ====================================
class DataHolder {
public:
    unique_ptr<Statements> statements = nullptr;
};

}

}

#endif /* YANodes_hpp */
