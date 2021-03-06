//
//  LegacyTypeScheat.cpp
//  scheat
//
//  Created by かずちか on 2020/08/09.
//

//#include "LegacyTypeScheat.hpp"
#include "scheat.h"
#include "scheatPriv.hpp"
#include "Lexer.hpp"
#include "ScheatContext.h"
//#include "ScheatNodes.h"
#include "ScheatStatics.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <map>

#define p_unique(id) std::unique_ptr<id>
#define make_p(id) std::make_unique<id>

using namespace std;
//using namespace scheat::LegacyScheatParser;
using namespace scheat;
//using namespace scheat::node;

//static NodeData* castType(IRStream &f, NodeData *data, TypeData *to){
//    if (data->size.ir_used[0] == '%') {
//        auto a = global_context->findClass(data->size.name);
//        if (a == nullptr) {
//            scheato->FatalError(__FILE_NAME__, __LINE__, "Domestic Error. %s is undefined.", data->size.ir_used.c_str());
//        }
//        if (a->type->name == to->name) {
//            auto r = local_context.top()->getRegister();
//            f << r << " = bitcast " << data->size.ir_used << "* " << data->value << " to " << to->ir_used << "*\n";
//        }
//    }
//    if (data->size.ir_used[0] == 'i') {
//
//    }
//    return nullptr;
//};

//Function::Function(std::string type, std::string nm) : return_type(type){
//    mangledName = "@" + local_context.top()->name + "_" + nm;
//    name = nm;
//    argTypes = {};
//    context = Context::create(nm, local_context.top());
//}

//void Context::addFunction(std::string key, Function *value){
//    funcs[key] = value;
//}

//void Context::addClass(std::string key, Class *value){
//    classes[key] = value;
//}

//void Context::addVariable(std::string key, Variable *value){
//    variables[key] = value;
//}

//bool Context::isExists(std::string key){
//    if (variables[key] != nullptr) {
//        return true;
//    }
//    if (funcs[key] != nullptr) {
//        return true;
//    }
//    return false;
//}

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
    mTokens = mTokens->next;
};

Context *Context::create(std::string name, Context *parents){
    Context *cnt = new Context(name, parents);
    if (parents == nullptr) {
        ScheatContext::contextCenter.push_back(cnt);
    }
    return cnt;
}

void LegacyScheatParser::E9::InitializeContexts(){
    ScheatContext::global = new Context();
    ScheatContext::global->name = "Global";
    ScheatContext::push(ScheatContext::global);
    ScheatContext::contextCenter.push_back(ScheatContext::global);
    ScheatContext::global->stream_entry << "source_filename = \"" << scheato->sourceFile << "\"\n";
    ScheatContext::global->stream_entry << "target datalayout = \"" << scheato->datalayout << "\"\n";
    ScheatContext::global->stream_entry << "target triple = \"" << scheato->target << "\"\n\n";
    ScheatContext::global->stream_body << "%ScheatShortConstString = type{ i8, [7 x i8] }\n";
    ScheatContext::global->stream_body << "%Array = type{ i32, i64, i8* }\n";
    ScheatContext::global->stream_body << "%union_buf = type{ %ScheatShortConstString }\n";
    ScheatContext::global->stream_body << "%String = type{ %union_buf }\n";
    ScheatContext::global->stream_body << "declare void @print_return()\n";
    ScheatContext::global->stream_body << "declare void @print_i32(i32)\n";
    ScheatContext::global->stream_body << "declare void @print_i8x(i8*)\n";
    ScheatContext::global->stream_body << "declare void @printn()\n";
    ScheatContext::global->stream_body << "declare void @print_i1(i1)\n";
    ScheatContext::global->stream_body << "declare i8* @ScheatPointer_alloc(i64, void(i8*)*)\n";
    ScheatContext::global->stream_body << "declare void @ScheatPointer_copy(i8*)\n";
    ScheatContext::global->stream_body << "declare void @ScheatPointer_unref(i8*)\n";
    ScheatContext::global->stream_body << "declare void @ScheatPointer_release(i8*)\n";
    ScheatContext::global->stream_body << "declare void @print_String(%String*)\n";
    ScheatContext::global->stream_body << "declare %String @String_init(i8*)\n";
    ScheatContext::global->stream_body << "declare %String* @String_add(%String*, %String*)\n";
    ScheatContext::global->stream_body << "declare %String* @String_copy(%String*)\n";
    ScheatContext::global->stream_body << "declare void @Array_append(%Array*, i8*)\n";
    ScheatContext::global->stream_body << "declare i8* @Array_at(%Array*, i32)\n";
    ScheatContext::global->stream_body << "declare %Array @Array_init(i64)\n";
    //global_context->stream_body << "declare i8* ";
    ScheatContext::main = nullptr;
    mTokens = nullptr;
    
    auto Int = new Class(new TypeData("i32"));
    Int->context->name = "Int";
    ScheatContext::global->addClass("Int", Int);
    auto String = new Class(new TypeData("String"));
    ScheatContext::global->addClass("String", String);
}

void LegacyScheatParser::E9::CreateMainContext(){
    Function *mainf = new Function("i32", "main");
    mainf->argTypes.push_back(TypeData("i32"));
    mainf->argTypes.push_back(TypeData("i8**"));
    mainf->return_type = TypeData("i32");
    ScheatContext::main = mainf->context;
    ScheatContext::contextCenter.push_back(ScheatContext::main);
    Variable *argc = new Variable("argc", TypeData("i32"));
    ScheatContext::main->addVariable("argc", argc);
    Variable *argv = new Variable("argv", TypeData("i8**"));
    ScheatContext::main->addVariable("argv", argv);
    mainf->codegen(ScheatContext::main->stream_entry);
    ScheatContext::main->stream_body << "entry:\n";
    ScheatContext::main->stream_body << "%argc = alloca i32\n";
    ScheatContext::main->stream_body << "%argv = alloca i8**\n";
    ScheatContext::main->stream_body << "store i32 %0, i32* %argc\n";
    ScheatContext::main->stream_body << "store i8** %1, i8*** %argv\n";
    ScheatContext::main->stream_tail << "ret i32 0\n}\n";
}

p_unique(node::Term) node::Term::create(std::unique_ptr<node::TermNode> term){
    p_unique(Term) t = std::make_unique<Term>();
    t->opTok = nullptr;
    t->terms = nullptr;
    t->node = std::move(term);
    t->node_size = term->node_size;
    return t;
};

p_unique(node::Term) node::Term::create(std::unique_ptr<Term> term, scheat::Token *opT, std::unique_ptr<TermNode> tn){
    p_unique(Term) t = std::make_unique<Term>();
    t->terms = move(term);
    t->opTok = opT;
    t->node = move(tn);
    t->node_size = tn->node_size;
    return t;
}

static node::NodeData *embbed_op_func_term_int(IRStream &f, node::NodeData *lhs, scheat::Token *tok, node::NodeData *rhs){
    if (lhs == nullptr) {
        // in this case, (OP Term) = prefix operator.
        scheato->FatalError(scheato->location, __FILE_NAME__, __LINE__, "Int(llvm i32) has no operator(%s)(Int)", tok->value.strValue.c_str());
    }
    if (rhs == nullptr) {
        // in this case, (Term OP) = postfix operator.
        if (tok->value.strValue != "!") {
            
        }
    }
    scheato->FatalError(scheato->location, __FILE_NAME__, __LINE__, "Unknown Error code %u", __LINE__);
    return nullptr;
}

node::NodeData *node::Term::codegen(IRStream &f){
    if (ident != nullptr) {
        return ident->codegen(f);
    }
    if (exprNode != nullptr) {
        return exprNode->codegen(f);
    }
    if (terms == nullptr && opTok == nullptr) {
        return node->codegen(f);
    }
    if (terms == nullptr && opTok != nullptr) {
        
    }
    return nullptr;
}

class TermIdentifier : public node::Term {
    scheat::Token *idTok;
public:
    __deprecated TermIdentifier(scheat::Token *t) : idTok(t), Term() {};
    node::NodeData *codegen(IRStream &) override;
};

node::NodeData *TermIdentifier::codegen(IRStream &f){
    
    return new NodeData(idTok->value.strValue, "VARIABLE");
}

class FunctionDefExpr : public ExprNode {
    std::vector<p_unique(PrototypeExpr)> args;
public:
    
};

class TermInt : public node::TermNode {
    scheat::Token *itok;
    
public:
    __deprecated TermInt(scheat::Token *t) : TermNode() {
        if (t->kind == scheat::TokenKind::val_num) {
            itok = t;
        }else if (t->kind == scheat::TokenKind::val_identifier){
            itok = t;
        }else{
            scheato->FatalError(location,__FILE_NAME__,
                               __LINE__,
                               "in %d.%d this is not suitable for int type.",
                               t->location.line,
                               t->location.column);
        }
    };
    node::NodeData * codegen(IRStream &) override;
    p_unique(TermInt) init(scheat::Token *i);
    ~TermInt() {};
};

p_unique(node::PrimaryExpr) node::PrimaryExpr::make(p_unique(node::Term) t){
    p_unique(PrimaryExpr) ex = std::make_unique<PrimaryExpr>();
    ex->exprs = nullptr;
    ex->opTok = nullptr;
    ex->term = std::move(t);
    ex->location = t->location;
    ex->node_size = t->node_size;
    return ex;
}

p_unique(node::PrimaryExpr) node::PrimaryExpr::make(std::unique_ptr<PrimaryExpr> e, scheat::Token *token, std::unique_ptr<Term> t){
    p_unique(PrimaryExpr) ex = std::make_unique<PrimaryExpr>();
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
        std::string r = ScheatContext::local()->getRegister();
        f << r << " = add nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new node::NodeData(r, "i32");
    }
    if (tok->value.strValue == "-") {
        std::string r1 = lhs->value;
        std::string r2 = rhs->value;
        std::string r = ScheatContext::local()->getRegister();
        f << r << " = sub nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new node::NodeData(r, "i32");
    }
    scheato->FatalError(scheato->location,__FILE_NAME__,
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
        std::string r = ScheatContext::local()->getRegister();
        f << r << " = imul nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new node::NodeData(r, "i32");
    }
    if (tok->value.strValue == "/") {
        std::string r1 = lhs->value;
        std::string r2 = rhs->value;
        std::string r = ScheatContext::local()->getRegister();
        f << r << " = idiv nsw i32 "
        << r1 << ", " << r2 << "\n";
        return new node::NodeData(r, "i32");
    }
    scheato->FatalError(scheato->location, __FILE_NAME__,
                       __LINE__,
                       "in %d.%d Int(llvm i32) does not have %s operator.",
                       tok->location.line,
                       tok->location.column,
                       tok->value.strValue.c_str());
    return nullptr;
}

node::NodeData *node::PrimaryExpr::codegen(IRStream &f){
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
        scheato->FatalError(location,__FILE_NAME__,
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
        auto ty = ScheatContext::global->findClass(k);
        if (ty == nullptr) {
            scheato->FatalError(location,__FILE_NAME__, __LINE__,
                                "%s is undefined.", k.c_str());
        }
        auto fu = ty->context->findFunc(opTok->encodeOperator());
        auto op = ty->operators[opTok->encodeOperator()];
        if (fu == nullptr) {
            scheato->FatalError(location,__FILE_NAME__, __LINE__,
                                "%s has no operator %s",
                                k.c_str(),
                                opTok->value.strValue.c_str()
                                );
            
        }
        
        if (fu->argTypes[0].ir_used != lhs->size.ir_used) {
            scheato->FatalError(location,__FILE_NAME__, __LINE__,
                                "%s needs %s type argument for left side.",
                                opTok->value.strValue.c_str(),
                                fu->argTypes[0].name.c_str());
        }
        
        if (fu->argTypes[0].ir_used != rhs->size.ir_used) {
            scheato->FatalError(location,__FILE_NAME__, __LINE__,
                                "%s needs %s type argument for left side.",
                                opTok->value.strValue.c_str(),
                                fu->argTypes[1].name.c_str());
        }
        
        auto r = ScheatContext::local()->getRegister();
        if (op->position == OperatorPosition::infix) {
            f << r << " = call " << fu->lltype() << " "
            << fu->getMangledName() << "(" << lhs->size.ir_used << "* " <<
            lhs->value << ", " << rhs->size.ir_used << "* " << rhs->value << ")\n";
        }
        
        if (op->position == OperatorPosition::prefix || op->position == OperatorPosition::postfix) {
            f << r << " call " << fu->lltype() << " " << fu->getMangledName() << "(" << lhs->size.ir_used << "* " << lhs->value << ")\n";
        }
        
        return new NodeData(r, fu->return_type.ir_used);
    }
    return nullptr;
}

class PrimaryExprInt : public node::Expr {
    p_unique(PrimaryExprInt) exprs;
    scheat::Token *opTok;
    p_unique(TermInt) term;
public:
    node::NodeData * codegen(IRStream &) override;
    __deprecated PrimaryExprInt(p_unique(TermInt) t,
                                scheat::Token *tok,
                                p_unique(PrimaryExprInt) e);
    
    static p_unique(PrimaryExprInt) init(p_unique(TermInt) t,
                                       scheat::Token *tok,
                                         p_unique(PrimaryExprInt) e);
    ~PrimaryExprInt() {};
};

PrimaryExprInt::PrimaryExprInt(p_unique(TermInt) t,
                               scheat::Token *tok,
                               p_unique(PrimaryExprInt) e){
    term = std::move(t);
    exprs = std::move(e);
    opTok = tok;
}

p_unique(PrimaryExprInt) PrimaryExprInt::init(p_unique(TermInt) t,
                                            scheat::Token *tok,
                                              p_unique(PrimaryExprInt) e){
    return std::make_unique<PrimaryExprInt>(std::move(t),
                                            tok,
                                            std::move(e));
}

class ExprInt : public ExprNode {
    p_unique(ExprInt) exprs;
    scheat::Token *opToken;
    p_unique(PrimaryExprInt) term;
public:
    node::NodeData * codegen(IRStream &) override;
    __deprecated ExprInt(p_unique(PrimaryExprInt) term,
                         scheat::Token *opTok = nullptr,
                         p_unique(ExprInt) exprs = nullptr);
    ~ExprInt() {};
    ExprInt() {};
};

ExprInt::ExprInt(p_unique(PrimaryExprInt) term,
                 scheat::Token *opTok,
                 p_unique(ExprInt) exprs){
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
            std::string r = ScheatContext::local()->getRegister();
            std::string k = exprs->codegen(f)->value;
            std::string l = term->codegen(f)->value;
            f << r << " = imul i32 " << k << ", " << l << "\n";
            return new node::NodeData(r, "i32");
        }
        if (opTok->value.strValue == "/") {
            std::string r = ScheatContext::local()->getRegister();
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

p_unique(node::IntTerm) node::IntTerm::make(Token *tok) {
    auto i = std::make_unique<IntTerm>();
    i->ident = nullptr;
    i->valToken = tok;
    return i;
}

p_unique(node::IntTerm) node::IntTerm::make(p_unique(IdentifierExpr) ie){
    auto i = std::make_unique<IntTerm>();
    i->ident = move(ie);
    i->valToken = nullptr;
    return i;
}

NodeData *node::IntTerm::codegen(IRStream &f){
    if (ident != nullptr) {
        auto k = ident->codegen(f);
        if (k->size.ir_used != "i32") {
            scheato->FatalError(location,__FILE_NAME__, __LINE__, "in %d.%d %s is not an Int type.",ident->location.line, ident->location.column, ident->userdump().c_str());
        }
    }else if (valToken != nullptr){
        
    }else{
        scheato->DevLog(location,__FILE_NAME__, __LINE__, "??");
    }
    return nullptr;
}

node::NodeData *TermInt::codegen(IRStream &f){
    if (itok == nullptr) {
        scheato->DevLog(location,__FILE_NAME__, __LINE__, "itok was null");
    }
    /*if (itok->kind == scheat::TokenKind::val_identifier) {
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
    }*/
    return new node::NodeData(std::to_string(itok->value.intValue), TypeData("Int", "i32"));
}

p_unique(TermInt) TermInt::init(scheat::Token *i){
    if (i->kind != scheat::TokenKind::val_num &&
        i->kind != scheat::TokenKind::val_identifier) {
        scheato->FatalError(location,__FILE_NAME__,
                           __LINE__,
                           "in %d.%d illegal Node is shifted.",
                           i->location.line,
                           i->location.column);
        return nullptr;
    }
    return std::make_unique<TermInt>(i);
}

p_unique(node::IdentifierTerm) node::IdentifierTerm::create(std::string v, bool n){
    auto cond = ScheatContext::local()->isExists(v);
    if (cond && !n) {
        scheato->FatalError(scheato->location,__FILE_NAME__, __LINE__, "%s already exists.", v.c_str());
    }
    if (!cond && !n) {
        scheato->FatalError(scheato->location,__FILE_NAME__, __LINE__, "%s does not exists.", v.c_str());
    }
    auto o = std::make_unique<IdentifierTerm>(v);
    return o;
}

p_unique(PrintStatement) PrintStatement::make(std::unique_ptr<Expr> expr){
    auto ob = std::make_unique<PrintStatement>();
    ob->ex = std::move(expr);
    ob->node_size.ir_used = "void";
    ob->node_size.name = "Void";
    return ob;
}

void PrintStatement::dump(IRStream &f){
    scheato->Log(location,__FILE_NAME__, __LINE__, "jump print");
}

NodeData *node::IdentifierExpr::codegen(IRStream &f){
    if (expr == nullptr) {
        auto v = ScheatContext::local()->findVariable(term->codegen(f)->value);
        if (v == nullptr) {
            // when the variable is undefined
            scheato->FatalError(location,__FILE_NAME__, __LINE__, "in %d.%d %s is undefined.", term->location.line, term->location.column, term->codegen(f)->value.c_str());
        }
        auto r = ScheatContext::local()->getRegister();
        f << r << " load " << v->type.ir_used << ", "
        << v->type.ir_used << "* " << v->mangledName << "\n";
        return new NodeData(r, v->type);
    }
    if (opTok == nullptr) {
        return nullptr;
    }
    auto d = expr->codegen(f);
    if (d == nullptr || scheato->hasProbrem()) {
        return nullptr;
    }
    
    if (opTok->kind == TokenKind::tok_period) {
        auto pt = d->size.name;
        auto ptclass = ScheatContext::local()->findClass(pt);
        if (ptclass == nullptr) {
            scheato->FatalError(location,__FILE_NAME__, __LINE__, "in %d.%d %s is undefined.", expr->location.line, expr->location.column, pt.c_str());
        }
        bool isVar = true;
        if (ptclass->properties.find(term->codegen(f)->value) == ptclass->properties.end()) {
            isVar = false;
        }
        if (!isVar && ptclass->context->findFunc(term->codegen(f)->value) == nullptr){
            scheato->FatalError(location,__FILE_NAME__, __LINE__, "in %d.%d %s has no function or methods named %s",
                                expr->location.line,
                                expr->location.column,
                                d->size.name.c_str(),
                                term->value.c_str());
        }
        if (isVar){
            //int index = ptclass->properties[term->codegen(f)->value];
            
        }else{
            
        }
    }
    
    return nullptr;
}

NodeData *node::IdentifierTerm::codegen(IRStream &f){
    if (!isFunction) {
        return new NodeData(value, "VARIABLE");
    }else{
        return new NodeData(value, "FUNCTION");
    }
    
}

NodeData *PrintStatement::codegen(IRStream &f){
    auto data = ex->codegen(f);
    if (data->size.ir_used == "i32") {
        f << "call void @print_i32(i32 " << data->value << "\n";
        f << "call void @printn()" << "\n";
    }
    
    return nullptr;
}

NodeData *node::Statements::codegen(IRStream &f){
    stmts->codegen(f);
    stmt->codegen(f);
    return nullptr;
}

p_unique(node::Statements) node::Statements::make(p_unique(StatementNode) st, p_unique(Statements) sts){
    auto obj = std::make_unique<Statements>();
    obj->stmt = move(st);
    obj->stmts = move(sts);
    return obj;
}

p_unique(node::Expr) node::Expr::make(std::unique_ptr<PrimaryExpr> lhs, Token *opTok, p_unique(Expr) rhs){
    auto e = std::make_unique<Expr>();
    e->body = move(lhs);
    e->node_size = e->body->node_size;
    if (opTok != nullptr) {
        e->op = opTok;
    }
    if (rhs != nullptr) {
        e->exprs = move(rhs);
    }
    
    return e;
}

p_unique(node::Expr) node::Expr::make(std::unique_ptr<Expr> expr, Token *op){
    auto e = std::make_unique<Expr>();
    e->exprs = move(expr);
    e->op = op;
    return e;
}

p_unique(node::Term) node::Term::create(std::unique_ptr<Expr> parenExpr){
    auto k = std::make_unique<Term>();
    k->opTok = nullptr;
    k->terms = nullptr;
    k->exprNode = move(parenExpr);
    return k;
}

NodeData *node::Expr::codegen(IRStream &f){
    if (exprs != nullptr && op != nullptr) {
        auto r = ScheatContext::local()->getRegister();
        auto nd = exprs->codegen(f);
        auto cl = ScheatContext::local()->findClass(nd->size.name);
        if (cl == nullptr) {
            scheato->FatalError(location,__FILE_NAME__, __LINE__,
                                "%s does not have operator %s",
                                nd->size.name.c_str(),
                                op->value.strValue.c_str());
        }
        auto Oper = cl->operators[op->value.strValue];
        f << r << " = call " << Oper->return_type.ir_used << "* ("
        << nd->size.ir_used << "*) " << Oper->func_name << "("
        << nd->size.ir_used << "* " << nd->value << ")\n";
    }
    return nullptr;
}

p_unique(node::PrimaryExpr) node::PrimaryExpr::make(std::unique_ptr<PrimaryExpr> expr, Token *opt){
    auto u = std::make_unique<PrimaryExpr>();
    u->exprs = move(expr);
    u->opTok = opt;
    u->term = nullptr;
    return nullptr;
}

p_unique(node::Term) node::Term::create(std::unique_ptr<IdentifierExpr> id){
    auto t = make_p(Term)();
    t->ident = move(id);
    t->exprNode = nullptr;
    t->node = nullptr;
    t->opTok = nullptr;
    return t;
}

p_unique(node::IdentifierExpr) node::IdentifierExpr::create(std::unique_ptr<IdentifierTerm> term, Token *opTok, std::unique_ptr<IdentifierExpr> expr){
    auto k = make_p(IdentifierExpr)();
    k->expr = move(expr);
    k->opTok = opTok;
    k->term = move(term);
    return k;
}
