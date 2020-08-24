//
//  LegacyTypeScheat.cpp
//  scheat
//
//  Created by かずちか on 2020/08/09.
//

#include "LegacyTypeScheat.hpp"
#include "scheat.hpp"
#include "scheatPriv.hpp"
#include "Lexer.hpp"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <map>

#define unique(id) std::unique_ptr<id>

using std::move;
class TypeData {
    
public:
    std::string ir_used;
    std::string name;
    
    bool operator==(TypeData *rhs){
        return this->name == rhs->name
        && this->mangledName() == rhs->mangledName();
    }
    virtual std::string mangledName() const{ return ir_used; };
    TypeData(std::string nm){
        name = nm;
        ir_used = "%" + nm;
    }
};

class NodeData {
    
public:
    std::string value;
    std::string size;
    NodeData(std::string a, std::string b) : value(a), size(b) {};
};

class Context;

class sFunction{
    
public:
    std::string mangledName;
    std::string getName();
    TypeData return_type;
    std::vector<TypeData> argTypes;
    std::string lltype();
    Context *context;
    sFunction(std::string ,std::string);
};

std::string sFunction::getName(){
    std::string base = return_type.mangledName() + "_";
    for (int i = 0; i < argTypes.size(); i++) {
        base = base + argTypes[i].mangledName();
        
        if (i < argTypes.size()) {
            base = base + "_";
        }
    }
    return mangledName + base;
}

class Operator {
    
public:
    enum{
        prefix,
        infix,
        postfix
    } position;
    
    enum{
        primary,
        non_primary,
        term
    } precidence;
    
    TypeData return_type;
    // if postfix operator, this will be nullptr
    TypeData *rhs_type;
    // if prefix operator, this will be nullptr
    TypeData *lhs_type;
    std::string func_name;
};

class Class {
    std::map<std::string, unsigned int> properties;
    std::map<std::string, Operator> operators;
    
public:
    TypeData *type;
    Class(TypeData *ty) : type(ty){
        properties = {};
        operators = {};
    };
};

std::string sFunction::lltype(){
    std::string base = return_type.mangledName() + "(";
    for (int i = 0; i < argTypes.size(); i++) {
        base = base + argTypes[i].mangledName();
        
        if (i < argTypes.size()) {
            base = base + ", ";
        }
    }
    base = base + ")";
    return base;
}

class Variable {

public:
    std::string mangledName;
    // do NOT think about pointer. it shows what value it has.
    TypeData type;
    Variable(std::string s, TypeData t) : mangledName(s), type(t) {
        
    }
};

class IRStream {
public:
    std::vector<std::string> irs;
    IRStream operator <<(std::string v){
        irs.push_back(v);
        return *this;
    };
    IRStream operator <<(const char v[]){
        std::string vs(v);
        irs.push_back(vs);
        return *this;
    }
    void exportTo(std::ofstream &f);
    IRStream(){
        irs = {};
    };
};

class Context {
    unsigned int rnum;
    std::map<std::string, Variable *> variables;
    std::map<std::string, sFunction *> funcs;
    std::map<std::string , Class *> classes;
    Context(std::string name, Context *parents){
        variables = {};
        funcs = {};
        rnum = 0;
        base = parents;
        this->name = name;
    }
public:
    IRStream stream_entry;
    IRStream stream_body;
    IRStream stream_tail;
    std::string name;
    Context *base;
    Variable *findVariable(std::string);
    bool isExists(std::string);
    std::string getRegister();
    Context(){
        variables = {};
        funcs = {};
        rnum = 0;
        base = nullptr;
        name = "";
    }
    
    void addFunction(std::string, sFunction *);
    
    void addClass(std::string, Class *);
    
    void addVariable(std::string, Variable *);
    
    static Context *create(std::string name, Context *parents);
};

static std::vector<Context *> contextCenter = {};
static Context *global_context;
static Context *main_Context;
static std::stack<Context *> local_context;
static scheat::Token *gltokens;
static std::string fname = "";

sFunction::sFunction(std::string type, std::string nm) : return_type(type){
    mangledName = "%" + local_context.top()->name + "_main";
    argTypes = {};
    context = Context::create(nm, local_context.top());
}

void IRStream::exportTo(std::ofstream &f){
    for (int i = 0; i < irs.size(); i++) {
        f << irs[i];
    }
}

void Context::addFunction(std::string key, sFunction *value){
    funcs[key] = value;
}

void Context::addClass(std::string key, Class *value){
    classes[key] = value;
}

void Context::addVariable(std::string key, Variable *value){
    variables[key] = value;
}

bool Context::isExists(std::string key){
    if (variables[key] != nullptr) {
        return true;
    }
    if (funcs[key] != nullptr) {
        return true;
    }
    return false;
}

std::string Context::getRegister(){
    std::string v = "%r" + std::to_string(rnum);
    rnum++;
    return v;
}

Variable *Context::findVariable(std::string key){
    Variable *v = variables[key];
    if (v != nullptr) {
        return v;
    }else{
        if (base == nullptr) {
            return nullptr;
        }
        v = base->findVariable(key);
        return v;
    }
}


static void getNextTok(){
    gltokens = gltokens->next;
};

Context *Context::create(std::string name, Context *parents){
    Context *cnt = new Context(name, parents);
    contextCenter.push_back(cnt);
    return cnt;
}

void LegacyScheat::E9::InitializeContexts(){
    global_context = new Context();
    global_context->name = "Global";
    local_context.push(global_context);
    contextCenter.push_back(global_context);
    main_Context = nullptr;
    gltokens = nullptr;
    
    auto Int = new Class(new TypeData("i32"));
}

void LegacyScheat::E9::CreateMainContext(){
    sFunction *mainf = new sFunction("i32", "main");
    mainf->mangledName = global_context->name + "_main";
    mainf->argTypes.push_back(TypeData("i32"));
    mainf->return_type = TypeData("i8**");
    main_Context = mainf->context;
    Variable *argc = new Variable("argc", TypeData("i32"));
    main_Context->addVariable("argc", argc);
    Variable *argv = new Variable("argv", TypeData("i8**"));
    main_Context->addVariable("argv", argv);
}

class Node {
    
public:
    virtual NodeData* codegen(IRStream &) { return nullptr; };
    
    virtual ~Node() {};
};

class TermNode : public Node {
    
    
public:
    NodeData * codegen(IRStream &) override{ return nullptr; };
    virtual ~TermNode();
};

class PrimaryExprNode : public Node {
    
public:
    NodeData * codegen(IRStream &) override { return nullptr; };
    virtual ~PrimaryExprNode();
};

class ExprNode : public Node {
    
public:
    NodeData * codegen(IRStream &) override { return nullptr; };
    virtual ~ExprNode();
};

class Term : public Node {
    unique(Term) terms;
    scheat::Token *opTok;
    unique(TermNode) node;
public:
    NodeData * codegen(IRStream &) override;
    static unique(Term) create(unique(TermNode));
    static unique(Term) create(unique(Term), scheat::Token *, unique(TermNode));
};

unique(Term) Term::create(std::unique_ptr<TermNode> term){
    unique(Term) t = std::make_unique<Term>();
    t->opTok = nullptr;
    t->terms = nullptr;
    t->node = std::move(term);
    return t;
};

unique(Term) Term::create(std::unique_ptr<Term> term, scheat::Token *opT, std::unique_ptr<TermNode> tn){
    unique(Term) t = std::make_unique<Term>();
    t->terms = move(term);
    t->opTok = opT;
    t->node = move(tn);
    return t;
}

static NodeData *embbed_op_func_term_int(IRStream &f, NodeData *lhs, scheat::Token *tok, NodeData *rhs){
    if (lhs == nullptr) {
        // in this case, (OP Term) = prefix operator.
        scheat::FatalError(__FILE_NAME__, __LINE__, "Int(llvm i32) has no operator(%s)(Int)", tok->value.strValue.c_str());
    }
    if (rhs == nullptr) {
        // in this case, (Term OP) = postfix operator.
        if (tok->value.strValue != "!") {
            
        }
    }
    scheat::FatalError(__FILE_NAME__, __LINE__, "Unknown Error code %u", __LINE__);
    return nullptr;
}

NodeData *Term::codegen(IRStream &f){
    if (terms == nullptr && opTok == nullptr) {
        return node->codegen(f);
    }
    if (terms == nullptr && opTok != nullptr) {
        
    }
    return nullptr;
}

class Expr : public Node {
    
public:
    NodeData * codegen(IRStream &) override{ return nullptr; };
};

class PrototypeExpr : public ExprNode {
    scheat::Token *id;
    TypeData *type;
public:
    __deprecated PrototypeExpr(scheat::Token *t, TypeData *ty) : id(t), type(ty), ExprNode() {};
    
};

class Statement : public Node {
    
public:
    virtual void dump(IRStream &) {};
};

class TermIdentifier : public Term {
    scheat::Token *idTok;
public:
    __deprecated TermIdentifier(scheat::Token *t) : idTok(t), Term() {};
    NodeData *codegen(IRStream &) override;
};

NodeData *TermIdentifier::codegen(IRStream &f){
    
    return nullptr;
}

class FunctionExpr : public ExprNode {
    std::vector<unique(PrototypeExpr)> args;
public:
    
};

class TermInt : public TermNode {
    scheat::Token *itok;
    unique(Expr) func;
public:
    __deprecated TermInt(scheat::Token *t) : TermNode() {
        if (t->kind == scheat::TokenKind::val_num) {
            itok = t;
        }else if (t->kind == scheat::TokenKind::val_identifier){
            itok = t;
        }else{
            scheat::FatalError(__FILE_NAME__,
                               __LINE__,
                               "in %d.%d this is not suitable for int type.",
                               t->location.line,
                               t->location.column);
        }
    };
    NodeData * codegen(IRStream &) override;
    unique(TermInt) init(scheat::Token *i);
    ~TermInt() {};
};

class PrimaryExpr : public ExprNode {
    unique(PrimaryExpr) exprs;
    scheat::Token *opTok;
    unique(Term) term;
public:
    __deprecated PrimaryExpr();
    NodeData * codegen(IRStream &) override;
    static unique(PrimaryExpr) make(unique(Term));
    static unique(PrimaryExpr) make(unique(PrimaryExpr), scheat::Token *, unique(Term));
};

unique(PrimaryExpr) PrimaryExpr::make(unique(Term) t){
    unique(PrimaryExpr) ex = std::make_unique<PrimaryExpr>();
    ex->exprs = nullptr;
    ex->opTok = nullptr;
    ex->term = std::move(t);
    return ex;
}

unique(PrimaryExpr) PrimaryExpr::make(std::unique_ptr<PrimaryExpr> e, scheat::Token *token, std::unique_ptr<Term> t){
    unique(PrimaryExpr) ex = std::make_unique<PrimaryExpr>();
    ex->exprs = std::move(e);
    ex->opTok = token;
    ex->term = std::move(t);
    return ex;
}

static NodeData *subFunc_OpInt(IRStream &f, NodeData *lhs, scheat::Token *tok,NodeData *rhs){
    if (tok->value.strValue == "+") {
        std::string r1 = lhs->value;
        std::string r2 = rhs->value;
        std::string r = local_context.top()->getRegister();
        f << r << " = add nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new NodeData(r, "i32");
    }
    if (tok->value.strValue == "-") {
        std::string r1 = lhs->value;
        std::string r2 = rhs->value;
        std::string r = local_context.top()->getRegister();
        f << r << " = sub nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new NodeData(r, "i32");
    }
    scheat::FatalError(__FILE_NAME__,
                       __LINE__,
                       "in %d.%d Int(llvm i32) does not have %s operator.",
                       tok->location.line,
                       tok->location.column,
                       tok->value.strValue.c_str());
    return nullptr;
};

static NodeData *subFunc_OpInt_Primary(IRStream &f, NodeData *lhs, scheat::Token *tok, NodeData *rhs){
    if (tok->value.strValue == "*") {
        std::string r1 = lhs->value;
        std::string r2 = rhs->value;
        std::string r = local_context.top()->getRegister();
        f << r << " = imul nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new NodeData(r, "i32");
    }
    if (tok->value.strValue == "/") {
        std::string r1 = lhs->value;
        std::string r2 = rhs->value;
        std::string r = local_context.top()->getRegister();
        f << r << " = idiv nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new NodeData(r, "i32");
    }
    scheat::FatalError(__FILE_NAME__,
                       __LINE__,
                       "in %d.%d Int(llvm i32) does not have %s operator.",
                       tok->location.line,
                       tok->location.column,
                       tok->value.strValue.c_str());
    return nullptr;
}

NodeData *PrimaryExpr::codegen(IRStream &f){
    if (exprs == nullptr) {
        return term->codegen(f);
    }
    
    if (opTok != nullptr && exprs == nullptr) {
        auto rhs = term->codegen(f);
        return subFunc_OpInt(f, nullptr, opTok, rhs);
    }
    
    auto lhs = exprs->codegen(f);
    auto rhs = term->codegen(f);
    if (lhs == nullptr || rhs == nullptr) {
        scheat::FatalError(__FILE_NAME__,
                           __LINE__,
                           "SystemError. error code: %u",
                           scheat::ScheatError::ERR_node_has_illegal_value);
        return nullptr;
    }
    if (lhs->size == "i32" &&
        rhs->size == "i32") {
        return subFunc_OpInt_Primary(f, lhs, opTok, rhs);
    }
    if (lhs->size[0] == '%') {
        
    }
    return nullptr;
}

class PrimaryExprInt : public Expr {
    unique(PrimaryExprInt) exprs;
    scheat::Token *opTok;
    unique(TermInt) term;
public:
    NodeData * codegen(IRStream &) override;
    __deprecated PrimaryExprInt(unique(TermInt) t,
                                scheat::Token *tok,
                                unique(PrimaryExprInt) e);
    
    static unique(PrimaryExprInt) init(unique(TermInt) t,
                                       scheat::Token *tok,
                                       unique(PrimaryExprInt) e);
    ~PrimaryExprInt() {};
};

PrimaryExprInt::PrimaryExprInt(unique(TermInt) t,
                               scheat::Token *tok,
                               unique(PrimaryExprInt) e){
    term = std::move(t);
    exprs = std::move(e);
    opTok = tok;
}

unique(PrimaryExprInt) PrimaryExprInt::init(unique(TermInt) t,
                                            scheat::Token *tok,
                                            unique(PrimaryExprInt) e){
    return std::make_unique<PrimaryExprInt>(std::move(t),
                                            tok,
                                            std::move(e));
}

class ExprInt : public ExprNode {
    unique(ExprInt) exprs;
    scheat::Token *opToken;
    unique(PrimaryExprInt) term;
public:
    NodeData * codegen(IRStream &) override;
    __deprecated ExprInt(unique(PrimaryExprInt) term,
                         scheat::Token *opTok = nullptr,
                         unique(ExprInt) exprs = nullptr);
    ~ExprInt();
};

ExprInt::ExprInt(unique(PrimaryExprInt) term,
                 scheat::Token *opTok,
                 unique(ExprInt) exprs){
    this->term = std::move(term);
    this->opToken = opTok;
    this->exprs = std::move(exprs);
}

NodeData *ExprInt::codegen(IRStream &f){
    if (opToken == nullptr) {
        return term->codegen(f);
    }else{
        
    }
    return nullptr;
}

NodeData *PrimaryExprInt::codegen(IRStream &f){
    if (opTok == nullptr) {
        return term->codegen(f);
    }else{
        if (opTok->value.strValue == "*") {
            std::string r = local_context.top()->getRegister();
            std::string k = exprs->codegen(f)->value;
            std::string l = term->codegen(f)->value;
            f << r << " = imul i32 " << k << ", " << l << "\n";
            return new NodeData(r, "i32");
        }
        if (opTok->value.strValue == "/") {
            std::string r = local_context.top()->getRegister();
            std::string k = exprs->codegen(f)->value;
            std::string l = term->codegen(f)->value;
            f << r << " = idiv i32 " << k << ", " << l << "\n";
            return new NodeData(r, "i32");
        }
        if (opTok->value.strValue == "!") {
            
        }
    }
    return nullptr;
}

NodeData *TermInt::codegen(IRStream &f){
    if (itok == nullptr) {
        return func->codegen(f);
    }
    if (itok->kind == scheat::TokenKind::val_identifier) {
        Variable *v = local_context.top()->findVariable(itok->value.strValue);
        if (v == nullptr) {
            scheat::FatalError(__FILE_NAME__,
                               __LINE__,
                               "in %d.%d %s is undefined.",
                               itok->location.line,
                               itok->location.column,
                               itok->value.strValue.c_str());
        }
        if (v->type.mangledName() != "i32") {
            scheat::FatalError(__FILE_NAME__,
                               __LINE__,
                               "in %d.%d %s is not an integer value.",
                               itok->location.line,
                               itok->location.column,
                               itok->value.strValue.c_str());
        }
        std::string r = local_context.top()->getRegister();
        f << r << " = load " << v->type.name << ", " << v->type.name << "* " << v->mangledName << "\n";
        return new NodeData(r, "i32");
    }
    return new NodeData(std::to_string(itok->value.intValue), "i32");
}

unique(TermInt) TermInt::init(scheat::Token *i){
    if (i->kind != scheat::TokenKind::val_num &&
        i->kind != scheat::TokenKind::val_identifier) {
        scheat::FatalError(__FILE_NAME__,
                           __LINE__,
                           "in %d.%d illegal Node is shifted.",
                           i->location.line,
                           i->location.column);
        return nullptr;
    }
    return std::make_unique<TermInt>(i);
}

// ----------------------------------------------------------------------------------------------------------------------------



unique(Expr) parseExpr(){
    if (gltokens->kind == scheat::TokenKind::val_operator) {
        if (gltokens->value.strValue == "!") {
            // unique(BoolExpr) parseBoolExpr();
        }
    }
    
    return nullptr;
}

unique(Statement) parseStatement(){
    if (gltokens->kind == scheat::TokenKind::embbed_func_print) {
        
    }
    return nullptr;
};

void LegacyScheat::Parse(scheat::Token *tokens, std::ofstream &f){
    E9::InitializeContexts();
    gltokens = tokens;
    unique(Statement) stmt = nullptr;
    while (stmt != nullptr) {
        stmt->codegen(local_context.top()->stream_body);
        stmt = nullptr;
    }
}
