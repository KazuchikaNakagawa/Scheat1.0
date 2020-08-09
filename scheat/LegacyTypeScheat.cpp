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
};

class Context {
    Context *base;
    unsigned int rnum;
    std::map<std::string, Variable *> variables;
public:
    Variable *findVariable(std::string);
    std::string getRegister();
};

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

static Context *local_context;

class Node {
    
public:
    virtual NodeData* codegen(std::ofstream&) { return nullptr; };
};

class TermInt : public Node {
    scheat::Token *itok;
public:
    __deprecated TermInt(scheat::Token *t) : itok(t) {};
    NodeData * codegen(std::ofstream&) override;
    unique(TermInt) init(scheat::Token *i);
    
};

class PrimaryExprInt : public Node {
    unique(PrimaryExprInt) exprs;
    scheat::Token *opTok;
    unique(TermInt) term;
public:
    NodeData * codegen(std::ofstream &) override;
};

NodeData *PrimaryExprInt::codegen(std::ofstream &f){
    
    return nullptr;
}

NodeData *TermInt::codegen(std::ofstream &f){
    if (itok->kind == scheat::TokenKind::val_identifier) {
        Variable *v = local_context->findVariable(itok->value.strValue);
        if (v == nullptr) {
            scheat::Scheat().FatalError(__LINE__, "in %d.%d %s is undefined.", itok->location.line, itok->location.column, itok->value.strValue.c_str());
        }
        if (v->type.size != "i32") {
            scheat::Scheat().FatalError(__LINE__, "in %d.%d %s is not an integer value.",  itok->location.line, itok->location.column, itok->value.strValue.c_str());
        }
        std::string r = local_context->getRegister();
        f << r << " = load " << v->type.name << std::endl;
        return new NodeData(r, "i32");
    }
    return new NodeData(std::to_string(itok->value.intValue), "i32");
}

unique(TermInt) TermInt::init(scheat::Token *i){
    if (i->kind != scheat::TokenKind::val_num &&
        i->kind != scheat::TokenKind::val_identifier) {
        scheat::Scheat().FatalError(__LINE__, "in %d.%d illegal Node is shifted.", i->location.line, i->location.column);
        return nullptr;
    }
    return std::make_unique<TermInt>(i);
}


