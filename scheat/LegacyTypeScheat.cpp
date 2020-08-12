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
    std::string size;
};

class Variable {

public:
    std::string mangledName;
    // do NOT think about pointer. it shows what value it has.
    TypeData type;
    Variable(std::string s, TypeData t) : mangledName(s), type(t) {
        
    }
};

class Context {
    Context *base;
    unsigned int rnum;
    std::map<std::string, Variable *> variables;
public:
    Variable *findVariable(std::string);
    std::string getRegister();
};

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

static std::stack<Context> local_context;

class Node {
    
public:
    virtual NodeData* codegen(std::ofstream&) { return nullptr; };
    
    virtual ~Node() {};
};

class TermInt : public Node {
    scheat::Token *itok;
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
    NodeData * codegen(std::ofstream&) override;
    unique(TermInt) init(scheat::Token *i);
    ~TermInt() {};
};

class PrimaryExprInt : public Node {
    unique(PrimaryExprInt) exprs;
    scheat::Token *opTok;
    unique(TermInt) term;
public:
    NodeData * codegen(std::ofstream &) override;
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
    NodeData * codegen(std::ofstream &) override;
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

NodeData *PrimaryExprInt::codegen(std::ofstream &f){
    if (opTok == nullptr) {
        return term->codegen(f);
    }else{
        if (opTok->value.strValue == "*") {
            std::string r = local_context.top().getRegister();
            std::string k = exprs->codegen(f)->value;
            std::string l = term->codegen(f)->value;
            f << r << " = imul i32 " << k << ", " << l << std::endl;
            return new NodeData(r, "i32");
        }
        if (opTok->value.strValue == "/") {
            std::string r = local_context.top().getRegister();
            std::string k = exprs->codegen(f)->value;
            std::string l = term->codegen(f)->value;
            f << r << " = idiv i32 " << k << ", " << l << std::endl;
            return new NodeData(r, "i32");
        }
    }
    return nullptr;
}

NodeData *TermInt::codegen(std::ofstream &f){
    if (itok->kind == scheat::TokenKind::val_identifier) {
        Variable *v = local_context.top().findVariable(itok->value.strValue);
        if (v == nullptr) {
            scheat::FatalError(__LINE__,
                               "in %d.%d %s is undefined.",
                               itok->location.line,
                               itok->location.column,
                               itok->value.strValue.c_str());
        }
        if (v->type.size != "i32") {
            scheat::FatalError(__LINE__,
                               "in %d.%d %s is not an integer value.",
                               itok->location.line,
                               itok->location.column,
                               itok->value.strValue.c_str());
        }
        std::string r = local_context.top().getRegister();
        f << r << " = load " << v->type.name << ", " << v->type.name << "* " << v->mangledName << std::endl;
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


