//
//  LegacyTypeScheat.cpp
//  scheat
//
//  Created by かずちか on 2020/08/09.
//

#include "LegacyTypeScheat.hpp"
#include "scheat.h"
#include "scheatPriv.hpp"
#include "Lexer.hpp"
#include "ScheatContext.h"
#include "ScheatNodes.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <map>

#define unique(id) std::unique_ptr<id>

using namespace scheat::LegacyScheatParser;
using namespace scheat;
using namespace scheat::basicStructs;
using namespace scheat::node;

Class *Context::findClass(std::string key){
    return classes[key];
}

Function *Context::findFunc(std::string key){
    return funcs[key];
}

using std::move;

static Scheat *scheato = nullptr;

std::string Function::getMangledName(){
    std::string base = return_type.mangledName() + "_";
    for (int i = 0; i < argTypes.size(); i++) {
        base = base + argTypes[i].mangledName();
        
        if (i < argTypes.size()) {
            base = base + "_";
        }
    }
    return mangledName + base;
}

std::string basicStructs::Function::lltype(){
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

void Context::dump(std::ofstream &f){
    
    // typename std::map<std::string, Class *>::iterator
    auto iter = begin(classes);
    while (iter != classes.end()) {
        auto pair = *iter;
        pair.second->context->dump(f);
        iter = std::next(iter);
    }
    f << "; " << name << "\n";
    stream_entry.exportTo(f);
    stream_body.exportTo(f);
    stream_tail.exportTo(f);
    
    auto iter_f = begin(funcs);
    while (iter_f != funcs.end()) {
        auto pair = *iter_f;
        pair.second->context->dump(f);
    }
    
    for (auto p = funcs.begin(); p != funcs.end(); p = std::next(p)) {
        (*p).second->context->dump(f);
    }
    
}

static std::vector<Context *> contextCenter = {};
static Context *global_context;
static Context *main_Context;
static std::stack<Context *> local_context;
static scheat::Token *gltokens;
static std::map<int, std::vector<std::string>> objects;
static std::string fname = "";

static NodeData* castType(IRStream &f, NodeData *data, TypeData *to){
    if (data->size.ir_used[0] == '%') {
        auto a = global_context->findClass(data->size.name);
        if (a == nullptr) {
            scheato->FatalError(__FILE_NAME__, __LINE__, "Domestic Error. %s is undefined.", data->size.ir_used.c_str());
        }
        if (a->type->name == to->name) {
            auto r = local_context.top()->getRegister();
            f << r << " = bitcast " << data->size.ir_used << "* " << data->value << " to " << to->ir_used << "\n";
        }
    }
    if (data->size.ir_used[0] == 'i') {
        
    }
    return nullptr;
};

Function::Function(std::string type, std::string nm) : return_type(type){
    mangledName = "@" + local_context.top()->name + "_" + nm;
    name = nm;
    argTypes = {};
    context = Context::create(nm, local_context.top());
}

void IRStream::exportTo(std::ofstream &f){
    for (int i = 0; i < irs.size(); i++) {
        f << irs[i];
    }
}

void Context::addFunction(std::string key, Function *value){
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
    return cnt;
}

void LegacyScheatParser::E9::InitializeContexts(){
    global_context = new Context();
    global_context->name = "Global";
    local_context.push(global_context);
    contextCenter.push_back(global_context);
    global_context->stream_entry << "source_filename = \"" << scheato->sourceFile << "\"\n";
    global_context->stream_entry << "target datalayout = \"" << scheato->datalayout << "\"\n";
    global_context->stream_entry << "target triple = \"" << scheato->target << "\"\n\n";
    global_context->stream_body << "declare void @printn()\n";
    global_context->stream_body << "declare void @print_i32(i32)\n";
    global_context->stream_body << "declare i8* @ScheatPointer_alloc(i64, void(i8*)*)\n";
    //global_context->stream_body << "declare i8* ";
    main_Context = nullptr;
    gltokens = nullptr;
    
    auto Int = new Class(new TypeData("i32"));
    Int->context->name = "Int";
    global_context->addClass("Int", Int);
    auto String = new Class(new TypeData("String"));
    global_context->addClass("String", String);
}

void LegacyScheatParser::E9::CreateMainContext(){
    Function *mainf = new Function("i32", "main");
    mainf->argTypes.push_back(TypeData("i32"));
    mainf->argTypes.push_back(TypeData("i8**"));
    mainf->return_type = TypeData("i32");
    main_Context = mainf->context;
    contextCenter.push_back(main_Context);
    Variable *argc = new Variable("argc", TypeData("i32"));
    main_Context->addVariable("argc", argc);
    Variable *argv = new Variable("argv", TypeData("i8**"));
    main_Context->addVariable("argv", argv);
    mainf->codegen(main_Context->stream_entry);
    main_Context->stream_body << "%argc = alloca i32\n";
    main_Context->stream_body << "%argv = alloca i8**\n";
    main_Context->stream_body << "store i32 %0, i32* %argc\n";
    main_Context->stream_body << "store i8** %1, i8*** %argv\n";
    main_Context->stream_tail << "ret i32 0\n}\n";
}

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

Class::Class(TypeData *ty) : type(ty){
    properties = {};
    operators = {};
    context = Context::create(ty->name, global_context);
};

static node::NodeData *embbed_op_func_term_int(IRStream &f, node::NodeData *lhs, scheat::Token *tok, node::NodeData *rhs){
    if (lhs == nullptr) {
        // in this case, (OP Term) = prefix operator.
        scheato->FatalError(__FILE_NAME__, __LINE__, "Int(llvm i32) has no operator(%s)(Int)", tok->value.strValue.c_str());
    }
    if (rhs == nullptr) {
        // in this case, (Term OP) = postfix operator.
        if (tok->value.strValue != "!") {
            
        }
    }
    scheato->FatalError(__FILE_NAME__, __LINE__, "Unknown Error code %u", __LINE__);
    return nullptr;
}

node::NodeData *Term::codegen(IRStream &f){
    if (terms == nullptr && opTok == nullptr) {
        return node->codegen(f);
    }
    if (terms == nullptr && opTok != nullptr) {
        
    }
    return nullptr;
}

class TermIdentifier : public Term {
    scheat::Token *idTok;
public:
    __deprecated TermIdentifier(scheat::Token *t) : idTok(t), Term() {};
    node::NodeData *codegen(IRStream &) override;
};

node::NodeData *TermIdentifier::codegen(IRStream &f){
    
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
            scheato->FatalError(__FILE_NAME__,
                               __LINE__,
                               "in %d.%d this is not suitable for int type.",
                               t->location.line,
                               t->location.column);
        }
    };
    node::NodeData * codegen(IRStream &) override;
    unique(TermInt) init(scheat::Token *i);
    ~TermInt() {};
};

unique(PrimaryExpr) PrimaryExpr::make(unique(Term) t){
    unique(PrimaryExpr) ex = std::make_unique<PrimaryExpr>();
    ex->exprs = nullptr;
    ex->opTok = nullptr;
    ex->term = std::move(t);
    ex->location = t->location;
    return ex;
}

unique(PrimaryExpr) PrimaryExpr::make(std::unique_ptr<PrimaryExpr> e, scheat::Token *token, std::unique_ptr<Term> t){
    unique(PrimaryExpr) ex = std::make_unique<PrimaryExpr>();
    ex->exprs = std::move(e);
    ex->opTok = token;
    ex->term = std::move(t);
    ex->location = token->location;
    return ex;
}

static node::NodeData *subFunc_OpInt(IRStream &f, node::NodeData *lhs, scheat::Token *tok,node::NodeData *rhs){
    if (tok->value.strValue == "+") {
        std::string r1 = lhs->value;
        std::string r2 = rhs->value;
        std::string r = local_context.top()->getRegister();
        f << r << " = add nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new node::NodeData(r, "i32");
    }
    if (tok->value.strValue == "-") {
        std::string r1 = lhs->value;
        std::string r2 = rhs->value;
        std::string r = local_context.top()->getRegister();
        f << r << " = sub nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new node::NodeData(r, "i32");
    }
    scheato->FatalError(__FILE_NAME__,
                       __LINE__,
                       "in %d.%d Int(llvm i32) does not have %s operator.",
                       tok->location.line,
                       tok->location.column,
                       tok->value.strValue.c_str());
    return nullptr;
};

static node::NodeData *subFunc_OpInt_Primary(IRStream &f, node::NodeData *lhs, scheat::Token *tok, node::NodeData *rhs){
    if (tok->value.strValue == "*") {
        std::string r1 = lhs->value;
        std::string r2 = rhs->value;
        std::string r = local_context.top()->getRegister();
        f << r << " = imul nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new node::NodeData(r, "i32");
    }
    if (tok->value.strValue == "/") {
        std::string r1 = lhs->value;
        std::string r2 = rhs->value;
        std::string r = local_context.top()->getRegister();
        f << r << " = idiv nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new node::NodeData(r, "i32");
    }
    scheato->FatalError(__FILE_NAME__,
                       __LINE__,
                       "in %d.%d Int(llvm i32) does not have %s operator.",
                       tok->location.line,
                       tok->location.column,
                       tok->value.strValue.c_str());
    return nullptr;
}

node::NodeData *PrimaryExpr::codegen(IRStream &f){
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
        scheato->FatalError(__FILE_NAME__,
                           __LINE__,
                           "SystemError. error code: %u",
                           scheat::ScheatError::ERR_node_has_illegal_value);
        return nullptr;
    }
    if (lhs->size.ir_used == "i32" &&
        rhs->size.ir_used == "i32") {
        return subFunc_OpInt_Primary(f, lhs, opTok, rhs);
    }
    if (lhs->size.ir_used[0] == '%') {
        std::string k = lhs->size.ir_used;
        k.erase(k.begin());
        auto ty = global_context->findClass(k);
        if (ty == nullptr) {
            scheato->FatalError(__FILE_NAME__, __LINE__,
                                "%s is undefined.", k.c_str());
        }
        auto fu = ty->context->findFunc(opTok->encodeOperator());
        if (fu == nullptr) {
            scheato->FatalError(__FILE_NAME__, __LINE__,
                                "%s has no operator %s",
                                k.c_str(),
                                opTok->value.strValue.c_str()
                                );
            
        }
        
        if (fu->argTypes[0].ir_used != lhs->size.ir_used) {
            scheato->FatalError(__FILE_NAME__, __LINE__,
                                "%s needs %s for left side.",
                                opTok->value.strValue.c_str(),
                                fu->argTypes[0].name.c_str());
        }
        
        if (fu->argTypes[0].ir_used != rhs->size.ir_used) {
            scheato->FatalError(__FILE_NAME__, __LINE__,
                                "%s needs %s for reft side.",
                                opTok->value.strValue.c_str(),
                                fu->argTypes[1].name.c_str());
        }
        
        auto r = local_context.top()->getRegister();
        f << r << " = call " << fu->lltype() << " "
        << fu->getMangledName() << "(" << lhs->size.ir_used << "* " <<
        lhs->value << ", " << rhs->size.ir_used << "* " << rhs->value << ")\n";
        return new NodeData(r, fu->return_type.ir_used);
    }
    return nullptr;
}

class PrimaryExprInt : public Expr {
    unique(PrimaryExprInt) exprs;
    scheat::Token *opTok;
    unique(TermInt) term;
public:
    node::NodeData * codegen(IRStream &) override;
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
    node::NodeData * codegen(IRStream &) override;
    __deprecated ExprInt(unique(PrimaryExprInt) term,
                         scheat::Token *opTok = nullptr,
                         unique(ExprInt) exprs = nullptr);
    ~ExprInt() {};
    ExprInt() {};
};

ExprInt::ExprInt(unique(PrimaryExprInt) term,
                 scheat::Token *opTok,
                 unique(ExprInt) exprs){
    this->term = std::move(term);
    this->opToken = opTok;
    this->exprs = std::move(exprs);
}

node::NodeData *ExprInt::codegen(IRStream &f){
    if (opToken == nullptr) {
        return term->codegen(f);
    }else{
        
    }
    return nullptr;
}

node::NodeData *PrimaryExprInt::codegen(IRStream &f){
    if (opTok == nullptr) {
        return term->codegen(f);
    }else{
        if (opTok->value.strValue == "*") {
            std::string r = local_context.top()->getRegister();
            std::string k = exprs->codegen(f)->value;
            std::string l = term->codegen(f)->value;
            f << r << " = imul i32 " << k << ", " << l << "\n";
            return new node::NodeData(r, "i32");
        }
        if (opTok->value.strValue == "/") {
            std::string r = local_context.top()->getRegister();
            std::string k = exprs->codegen(f)->value;
            std::string l = term->codegen(f)->value;
            f << r << " = idiv i32 " << k << ", " << l << "\n";
            return new node::NodeData(r, "i32");
        }
        if (opTok->value.strValue == "!") {
            
        }
    }
    return nullptr;
}

node::NodeData *TermInt::codegen(IRStream &f){
    if (func != nullptr) {
        auto data = func->codegen(f);
        if (scheato->hasProbrem()) {
            return nullptr;
        }
        if (data->size.ir_used != "i32*") {
            scheato->FatalError(__FILE_NAME__, __LINE__, "this is not a integer type.");
        }
        auto r = local_context.top()->getRegister();
        f << r << " = load i32, i32* " << data->value << "\n";
    }
    if (itok == nullptr) {
        return func->codegen(f);
    }
    if (itok->kind == scheat::TokenKind::val_identifier) {
        Variable *v = local_context.top()->findVariable(itok->value.strValue);
        if (v == nullptr) {
            scheato->FatalError(__FILE_NAME__,
                               __LINE__,
                               "in %d.%d %s is undefined.",
                               itok->location.line,
                               itok->location.column,
                               itok->value.strValue.c_str());
        }
        if (v->type.mangledName() != "i32") {
            scheato->FatalError(__FILE_NAME__,
                               __LINE__,
                               "in %d.%d %s is not an integer value.",
                               itok->location.line,
                               itok->location.column,
                               itok->value.strValue.c_str());
        }
        std::string r = local_context.top()->getRegister();
        f << r << " = load " << v->type.name << ", " << v->type.name << "* " << v->mangledName << "\n";
        return new node::NodeData(r, "i32");
    }
    return new node::NodeData(std::to_string(itok->value.intValue), "i32");
}

unique(TermInt) TermInt::init(scheat::Token *i){
    if (i->kind != scheat::TokenKind::val_num &&
        i->kind != scheat::TokenKind::val_identifier) {
        scheato->FatalError(__FILE_NAME__,
                           __LINE__,
                           "in %d.%d illegal Node is shifted.",
                           i->location.line,
                           i->location.column);
        return nullptr;
    }
    return std::make_unique<TermInt>(i);
}

unique(IdentifierTerm) IdentifierTerm::create(std::string v, bool n){
    auto cond = local_context.top()->isExists(v);
    if (cond && !n) {
        scheato->FatalError(__FILE_NAME__, __LINE__, "%s already exists.", v.c_str());
    }
    if (!cond && !n) {
        scheato->FatalError(__FILE_NAME__, __LINE__, "%s does not exists.", v.c_str());
    }
    auto o = std::make_unique<IdentifierTerm>(v);
    return o;
}

unique(PrintStatement) PrintStatement::make(std::unique_ptr<Expr> expr){
    auto ob = std::make_unique<PrintStatement>();
    ob->ex = std::move(expr);
    return ob;
}

void PrintStatement::dump(IRStream &f){
    scheato->Log(__FILE_NAME__, __LINE__, "jump print");
}

NodeData *PrintStatement::codegen(IRStream &f){
    auto data = ex->codegen(f);
    if (data->size.ir_used == "i32") {
        f << "call void @print_i32(i32 " << data->value << "\n";
    }
    f << "call void @printn()" << "\n";
    return nullptr;
}

NodeData *Statements::codegen(IRStream &f){
    stmts->codegen(f);
    stmt->codegen(f);
    return nullptr;
}

unique(Statements) Statements::make(unique(StatementNode) st, unique(Statements) sts = nullptr){
    auto obj = std::make_unique<Statements>();
    obj->stmt = move(st);
    obj->stmts = move(sts);
    return obj;
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

unique(StatementNode) parsePrintStatement(){
    getNextTok();
    auto expr = parseExpr();
    if (scheato->hasProbrem()) {
        return nullptr;
    }
    return PrintStatement::make(std::move(expr));
}

unique(StatementNode) parseStatement(){
    if (gltokens == nullptr) {
        return nullptr;
    }
    if (gltokens->kind == scheat::TokenKind::embbed_func_print) {
        return parsePrintStatement();
    }
    return nullptr;
};

unique(Statements) parseStatements(){
    if (gltokens == nullptr) {
        return nullptr;
    }
    auto s = parseStatement();
    if (scheato->hasProbrem() || s == nullptr) {
        return nullptr;
    }
    if (gltokens == nullptr) {
        scheato->FatalError(__FILE_NAME__, __LINE__, "illegal null token");
        return nullptr;
    }
    if (gltokens->kind == scheat::TokenKind::tok_comma) {
        auto ss = parseStatements();
        auto v = Statements::make(move(s), move(ss));
        return v;
    }else if (gltokens->kind == scheat::TokenKind::tok_period){
        getNextTok();
        return Statements::make(move(s));
    }
    
    return nullptr;
}

void LegacyScheatParser::Parse(Scheat *host, scheat::Token *tokens){
    scheato = host;
    std::ofstream f(host->outputFilePath + ".ll");
    if (!f.is_open()) {
        host->FatalError("", 0, "File not found");
        return;
    }
    E9::InitializeContexts();
    size_t len1 = host->sourceFile.size();
    size_t len2 = std::string(".scheat").size();
    if (len1 >= len2 && host->sourceFile.compare(len1 - len2, len2, ".scheat") == 0){;
        E9::CreateMainContext();
    }
    gltokens = tokens;
    unique(Statements) stmt = nullptr;
    while (stmt = parseStatements(),stmt != nullptr) {
        stmt->codegen(local_context.top()->stream_body);
        stmt = parseStatements();
    }
    for (auto i = contextCenter.begin(); i != contextCenter.end(); i = std::next(i)) {
        (*i)->dump(f);
        f << "\n";
    }
}
