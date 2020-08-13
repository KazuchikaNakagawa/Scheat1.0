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

class NodeData {
    
public:
    std::string value;
    std::string size;
    NodeData(std::string a, std::string b) : value(a), size(b) {};
};

class TypeData {
    
public:
    std::string name;
    std::string mangledName;
};

class Function{
public:
    std::string mangledName;
    TypeData return_type;
    std::vector<TypeData> argTypes;
    std::string lltype();
};

std::string Function::lltype(){
    std::string base = return_type.mangledName + "(";
    for (int i = 0; i < argTypes.size(); i++) {
        base = base + argTypes[i].mangledName;
        
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

void IRStream::exportTo(std::ofstream &f){
    for (int i = 0; i < irs.size(); i++) {
        f << irs[i];
    }
}

class Context {
    unsigned int rnum;
    std::map<std::string, Variable *> variables;
    std::map<std::string, Function *> funcs;
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
};

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
    std::string v = "%tmp" + std::to_string(rnum);
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

static std::vector<Context *> contextCenter = {};
static Context *global_context;
static std::stack<Context *> local_context;

void LegacyScheat::E9::InitializeContexts(){
    global_context = new Context();
    global_context->name = "glbl";
    local_context.push(global_context);
    contextCenter.push_back(global_context);
}

class Node {
    
public:
    virtual NodeData* codegen(IRStream &) { return nullptr; };
    
    virtual ~Node() {};
};

class PrototypeExpr : public Node {
    scheat::Token *id;
    TypeData *type;
public:
    __deprecated PrototypeExpr(scheat::Token *t, TypeData *ty) : id(t), type(ty), Node() {};
    
};

class Statement : public Node {
    
public:
    virtual void dump(IRStream &) {};
};

class TermIdentifier : public Node {
    scheat::Token *idTok;
public:
    __deprecated TermIdentifier(scheat::Token *t) : idTok(t), Node() {};
    NodeData *codegen(IRStream &) override;
};

NodeData *TermIdentifier::codegen(IRStream &f){
    
    return nullptr;
}

class FunctionExpr : public Node {
    std::vector<unique(PrototypeExpr)> args;
public:
    
};

class TermInt : public Node {
    scheat::Token *itok;
    unique(Node) func;
public:
    __deprecated TermInt(scheat::Token *t) : Node() {
        if (t->kind == scheat::TokenKind::val_num) {
            itok = t;
        }else if (t->kind == scheat::TokenKind::val_identifier){
            itok = t;
        }else{
            scheat::FatalError(__LINE__,
                               "in %d.%d this is not suitable for int type.",
                               t->location.line,
                               t->location.column);
        }
    };
    TermInt(const TermInt &r) : Node(r) {};
    TermInt(TermInt &&) = default;
    NodeData * codegen(IRStream &) override;
    unique(TermInt) init(scheat::Token *i);
    ~TermInt() {};
};

class PrimaryExprInt : public Node {
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

class ExprInt : public Node {
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
            scheat::FatalError(__LINE__,
                               "in %d.%d %s is undefined.",
                               itok->location.line,
                               itok->location.column,
                               itok->value.strValue.c_str());
        }
        if (v->type.mangledName != "i32") {
            scheat::FatalError(__LINE__,
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
        scheat::FatalError(__LINE__,
                           "in %d.%d illegal Node is shifted.",
                           i->location.line,
                           i->location.column);
        return nullptr;
    }
    return std::make_unique<TermInt>(i);
}

// ----------------------------------------------------------------------------------------------------------------------------

unique(Statement) parseStatement(scheat::Token *tokens){
    if (tokens->kind == scheat::TokenKind::embbed_func_print) {
        
    }
    return nullptr;
};

void LegacyScheat::Parse(scheat::Token *tokens, std::ofstream &f){
    E9::InitializeContexts();
    unique(Statement) stmt = nullptr;
    while (stmt != nullptr) {
        stmt->codegen(local_context.top()->stream_body);
        stmt = nullptr;
    }
}
