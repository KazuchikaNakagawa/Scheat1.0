//
//  ScheatNode.cpp
//  scheat
//
//  Created by かずちか on 2020/09/13.
//

#include <stdio.h>
//#include "ScheatNodes.h"
#include "ScheatStatics.h"
//#include "LegacyTypeScheat.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <stack>

//#define ScheatContextData ScheatContext

//using namespace scheat::LegacyScheatParser;
using namespace scheat;
//using namespace scheat::node;
using std::string;

static void getNextTok(){
    mTokens = mTokens->next;
};

static Token * BackwardIFExists(){
    auto tok = mTokens;
    if (tok->kind == scheat::TokenKind::tok_if) {
        scheato->DevLog(tok->location,__FILE_NAME__, __LINE__, "if token was detected but it seemed not to  be backward if");
        return nullptr;
    }
    while (tok->kind != scheat::TokenKind::tok_period) {
        if (tok->kind == scheat::TokenKind::tok_if) {
            return tok;
        }
        tok = tok->next;
    }
    return nullptr;
}

struct IDData {
    std::string valueName;
    TypeData type;
    IDData(string s, TypeData t) : valueName(s), type(t) {};
};

static IDData inferID(Token *&tok){
    if (tok->next->kind == scheat::TokenKind::tok_period) {
        
        Token *idt = tok;
        eatThis(tok);
        eatThis(tok);
        auto idd = inferID(tok);
        auto v = ScheatContext::local()->findVariable(idt->value.strValue);
        if (v == nullptr) {
            scheato->FatalError(idt->location, __FILE_NAME__, __LINE__, "identifier %s is not defined",
                                idt->value.strValue.c_str());
            exit(0);
        }
        auto t = ScheatContext::local()->findClass(v->type.name);
        if (t == nullptr) {
            exit(0);
        }
        // Error if the member function doesn't exists
        if (t->properties.find(idd.valueName) == t->properties.end()) {
            scheato->FatalError(idt->location,
                                __FILE_NAME__, __LINE__, "type %s is undefined",
                                idd.type.name.c_str());
            exit(0);
        }
        
        return idd;
        
    }else if (tok->kind == scheat::TokenKind::val_identifier){
        if (!ScheatContext::local()->isExists(tok->value.strValue)) {
            return IDData("",TypeData("nil", "NULLTYPE"));
        }
        auto v = ScheatContext::local()->findVariable(tok->value.strValue);
        if (v != nullptr) {
            return IDData(v->mangledName, v->type);
        }
        auto f = ScheatContext::local()->findFunc(tok->value.strValue);
        if (f != nullptr) {
            if (tok->next->kind == scheat::TokenKind::tok_paren_l) {
                int i = 0;
                while (tok->kind != scheat::TokenKind::tok_paren_r) {
                    tok = tok->next;
                    i++;
                    if (i > 300) {
                        scheato->FatalError(tok->location,
                                            __FILE_NAME__, __LINE__, ") token was not found in 300 token.");
                    }
                }
            }else{
                return IDData(f->name, TypeData("Function", f->lltype()));
            }
        }
    }else{
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__, "unknown member function or variable named %s", tok->value.strValue.c_str());
        return IDData("nil", TypeData("nil", "NULLTYPE"));
    }
    return IDData("nil", TypeData("nil", "NULLTYPE"));
}

static TypeData inferIDType(Token *&tok){
//    if (tok->next->kind == scheat::TokenKind::tok_period) {
//        Token *idTok = tok;
//        eatThis(tok);
//        eatThis(tok);
//
//    }
    if (!ScheatContext::local()->isExists(tok->value.strValue)) {
        return TypeData("nil", "NULLTYPE");
    }
    
    return inferID(tok).type;
}

static TypeData inferTermType(Token*& ktok){
    if (ktok->kind == scheat::TokenKind::val_num) {
        eatThis(ktok);
        return TypeData("Int", "i32");
    }
    
    if (ktok->kind == scheat::TokenKind::val_str) {
        eatThis(ktok);
        return TypeData("String", "%String");
    }
    
    if (ktok->kind == scheat::TokenKind::val_bool) {
        eatThis(ktok);
        return TypeData("Bool", "i1");
    }
    
    if (ktok->kind == scheat::TokenKind::val_double) {
        eatThis(ktok);
        return TypeData("Double", "double");
    }
    
    if (ktok->kind == scheat::TokenKind::val_identifier) {
        eatThis(ktok);
        return inferIDType(ktok);
    }
    return TypeData("nil", "NULLTYPE");
}

static TypeData inferPrimaryType(Token *&ktok){
    
    if (ktok->kind == scheat::TokenKind::val_operator) {
        ktok = ktok->next;
        auto t = inferTermType(ktok);
        auto cl = ScheatContext::global->findClass(t.name);
        if (cl == nullptr) {
            return TypeData("nil", "NULLTYPE");
        }
        auto opiter = cl->operators.find(ktok->value.strValue);
        if (opiter == cl->operators.end()) {
            return TypeData("nil", "NULLTYPE");
        }
        
        auto op = (*opiter).second;
        
        if (op->precidence != primary) {
            ktok = ktok->prev;
            return inferPrimaryType(ktok);
        }
        
        return op->return_type;
    }
    return inferTermType(ktok);
}

static TypeData inferType(){
    
    auto ktok = mTokens;
    
    if (ktok->kind == scheat::TokenKind::val_operator) {
        ktok = ktok->next;
        auto t = inferType();
        auto cl = ScheatContext::global->findClass(t.name);
        if (cl == nullptr) {
            return TypeData("nil", "NULLTYPE");
        }
        auto opiter = cl->operators.find(ktok->value.strValue);
        if (opiter == cl->operators.end()) {
            return TypeData("nil", "NULLTYPE");
        }
        
        auto op = (*opiter).second;
        
        if (op->precidence != secondary) {
            ktok = ktok->prev;
            return inferPrimaryType(ktok);
        }
        
        return op->return_type;
    }
    
    return inferPrimaryType(ktok);
    
    //gltokens->out();
    //scheato->FatalError(__FILE_NAME__, __LINE__, "<- failed to infer the type of token.");
    return TypeData("nil", "NULLTYPE");
}

extern p_unique(Expr) parseExpr(Token *&);

p_unique(IdentifierTerm) parseIdentifierTerm(Token *&gltokens){
    
    // idterm : identifier
    //        | identifier ( exprs )
    if (gltokens->kind != scheat::TokenKind::val_identifier) {
        scheato->DevLog(gltokens->location, __FILE_NAME__, __LINE__, "illegal parsing function is called.");
        return nullptr;
    }
    
    
    
    return nullptr;
}

p_unique(IdentifierExpr) parseIdentifierExpr_(Token *&gltokens){
    // idexpr : idterm
    //        | idexpr . idterm
    auto t = parseIdentifierTerm(gltokens);
    if (!t) {
        return nullptr;
    }
    return IdentifierExpr::create(move(t), nullptr, nullptr);
};

p_unique(IdentifierExpr) parseIdentifierExpr(Token *&gltokens){
    auto ptr = parseIdentifierExpr_(gltokens);
    if (!ptr) {
        return nullptr;
    }
    while (true) {
        if (gltokens->kind != scheat::TokenKind::tok_access) {
            break;
        }
        auto opt = gltokens;
        eatThis(gltokens);
        ptr = IdentifierExpr::create(parseIdentifierTerm(gltokens), opt, move(ptr));
    }
    return ptr;
}

p_unique(Term) parseTerm(Token*& gltokens){
    // term : int
    //      | str
    //      | bool
    //      | double
    //      | identifier_expr
    if (gltokens->kind == scheat::TokenKind::tok_paren_l) {
        eatThis(mTokens);
        auto e = parseExpr(gltokens);
        if (scheato->hasProbrem()) {
            return nullptr;
        }
        
        if (e == nullptr) {
            return nullptr;
        }
        
        if (mTokens->kind != scheat::TokenKind::tok_paren_r) {
            scheato->FatalError(mTokens->location, __FILE_NAME__, __LINE__, "here ) is needed.");
        }
        
        return Term::create(move(e));
    }
    if (gltokens->kind == scheat::TokenKind::val_identifier) {
        return Term::create(parseIdentifierExpr(gltokens));
    }
    return nullptr;
}

p_unique(PrimaryExpr) parsePrimaryExpr(Token *&gltokens){
    // expr : primary_expr
    //      | primary_expr OP expr
    //      | Op expr
    if (gltokens->kind == scheat::TokenKind::val_operator) {
        //  | Op primary_expr
        Token *opTok = gltokens;
        Token *copy = gltokens;
        eatThis(copy);
        auto primary = parsePrimaryExpr(copy);
        if (!primary) {
            return nullptr;
        }
        auto opOwner = ScheatContext::local()->findClass(primary->node_size.name);
        if (!opOwner) {
            scheato->DevLog(opTok->location, __FILE_NAME__, __LINE__, "????");
        }
        if (opOwner->operators.find(opTok->value.strValue) == opOwner->operators.end()) {
            scheato->FatalError(opTok->location, __FILE_NAME__, __LINE__, "in %d.%d %s does not have operator %s",
                                opTok->location.column,
                                opTok->location.line,
                                opOwner->context->name.c_str(),
                                opTok->value.strValue.c_str());
        }
        auto op = opOwner->operators[opTok->value.strValue];
        if (op->precidence != OperatorPresidence::primary) {
            goto symbol;
        }
        if (op->position != prefix) {
            scheato->FatalError(opTok->location, __FILE_NAME__, __LINE__,
                                "in %d.%d %s's operator %s is not a prefix operator.",
                                opTok->location.line,
                                opTok->location.column,
                                primary->node_size.name.c_str(),
                                opTok->value.strValue.c_str());
            
        }
        
        gltokens = copy;
        return PrimaryExpr::make(move(primary), opTok);
        
    }else{
    symbol:
        auto pri = parseTerm(gltokens);
        if (!pri) {
            return nullptr;
        }
        if (gltokens->kind == scheat::TokenKind::val_operator) {
            Token *opTok = gltokens;
            Token *copy = gltokens;
            eatThis(copy);
            auto e = parsePrimaryExpr(copy);
            if (!e) {
                return nullptr;
            }
            auto opOwner = ScheatContext::local()->findClass(e->node_size.name);
            if (!opOwner) {
                scheato->DevLog(opTok->location, __FILE_NAME__, __LINE__, "????");
            }
            if (opOwner->operators.find(opTok->value.strValue) == opOwner->operators.end()) {
                scheato->FatalError(opTok->location, __FILE_NAME__, __LINE__, "in %d.%d %s does not have operator %s",
                                    opTok->location.column,
                                    opTok->location.line,
                                    opOwner->context->name.c_str(),
                                    opTok->value.strValue.c_str());
            }
            auto op = opOwner->operators[opTok->value.strValue];
            if (op->precidence != OperatorPresidence::primary) {
                goto symbol;
            }
            if (op->position != infix) {
                scheato->FatalError(opTok->location, __FILE_NAME__, __LINE__,
                                    "in %d.%d %s's operator %s is not a infix operator.",
                                    opTok->location.line,
                                    opTok->location.column,
                                    e->node_size.name.c_str(),
                                    opTok->value.strValue.c_str());
                
            }
            
            gltokens = copy;
            return PrimaryExpr::make(move(e), opTok, move(pri));
        }
        
    }
    
    return nullptr;
}

p_unique(Expr) parseExpr(Token *&gltokens){
    // expr : primary_expr
    //      | primary_expr OP expr
    //      | Op expr
    if (gltokens->kind == scheat::TokenKind::val_operator) {
        //  | Op primary_expr
        Token *opTok = gltokens;
        Token *copy = gltokens;
        eatThis(copy);
        auto primary = parseExpr(copy);
        if (!primary) {
            return nullptr;
        }
        auto opOwner = ScheatContext::local()->findClass(primary->node_size.name);
        if (!opOwner) {
            scheato->DevLog(opTok->location, __FILE_NAME__, __LINE__, "????");
        }
        if (opOwner->operators.find(opTok->value.strValue) == opOwner->operators.end()) {
            scheato->FatalError(opTok->location, __FILE_NAME__, __LINE__, "in %d.%d %s does not have operator %s",
                                opTok->location.column,
                                opTok->location.line,
                                opOwner->context->name.c_str(),
                                opTok->value.strValue.c_str());
        }
        auto op = opOwner->operators[opTok->value.strValue];
        if (op->precidence != OperatorPresidence::secondary) {
            goto symbol;
        }
        if (op->position != infix) {
            scheato->FatalError(opTok->location, __FILE_NAME__, __LINE__,
                                "%s's operator %s is not a infix operator.",
                                primary->node_size.name.c_str(),
                                opTok->value.strValue.c_str());
            
        }
        
        gltokens = copy;
        return Expr::make(move(primary), opTok);
        
    }else{
        symbol:
        auto pri = parsePrimaryExpr(gltokens);
        if (!pri) {
            return nullptr;
        }
        if (gltokens->kind == scheat::TokenKind::val_operator) {
            Token *opTok = gltokens;
            Token *copy = gltokens;
            eatThis(copy);
            auto e = parseExpr(copy);
            if (!e) {
                return nullptr;
            }
            auto opOwner = ScheatContext::local()->findClass(e->node_size.name);
            if (!opOwner) {
                scheato->DevLog(opTok->location,__FILE_NAME__, __LINE__, "????");
            }
            if (opOwner->operators.find(opTok->value.strValue) == opOwner->operators.end()) {
                scheato->FatalError(opTok->location,__FILE_NAME__, __LINE__, "in %d.%d %s does not have operator %s",
                                    opTok->location.column,
                                    opTok->location.line,
                                    opOwner->context->name.c_str(),
                                    opTok->value.strValue.c_str());
            }
            auto op = opOwner->operators[opTok->value.strValue];
            if (op->precidence != secondary) {
                goto symbol;
            }
            if (op->position != prefix) {
                scheato->FatalError(opTok->location,__FILE_NAME__, __LINE__,
                                    "in %d.%d %s's operator %s is not a prefix operator.",
                                    opTok->location.line,
                                    opTok->location.column,
                                    e->node_size.name.c_str(),
                                    opTok->value.strValue.c_str());
                
            }
            
            gltokens = copy;
            return Expr::make(move(pri), opTok, move(e));
        }
        
    }
    
    return nullptr;
}

p_unique(StatementNode) parsePrintStatement(Token*& gltokens){
    getNextTok();
    auto expr = parseExpr(gltokens);
    if (scheato->hasProbrem()) {
        return nullptr;
    }
    return PrintStatement::make(std::move(expr));
}

p_unique(StatementNode) parseStatement(Token *&gltokens){
    if (gltokens == nullptr) {
        return nullptr;
    }
    if (gltokens->kind == scheat::TokenKind::embbed_func_print) {
        return parsePrintStatement(gltokens);
    }
    return nullptr;
};

p_unique(Statements) parseStatements(Token*& gltokens){
    if (gltokens == nullptr) {
        return nullptr;
    }
    auto s = parseStatement(gltokens);
    if (scheato->hasProbrem() || s == nullptr) {
        return nullptr;
    }
    if (mTokens == nullptr) {
        scheato->FatalError(gltokens->location,__FILE_NAME__, __LINE__, "illegal null token");
        return nullptr;
    }
    if (mTokens->kind == scheat::TokenKind::tok_comma) {
        auto ss = parseStatements(gltokens);
        auto v = Statements::make(move(s), move(ss));
        return v;
    }else if (gltokens->kind == scheat::TokenKind::tok_period){
        getNextTok();
        return Statements::make(move(s));
    }
    scheato->FatalError(gltokens->location, __FILE_NAME__, __LINE__, "in %d.%d illegal end of the statement", gltokens->location.line, gltokens->location.column);
    return nullptr;
}

void LegacyScheatParser::LLParse(_Scheat *host){
    scheato = host;
    std::ofstream f(host->outputFilePath + ".ll");
    if (!f.is_open()) {
        host->FatalError(SourceLocation(), __FILE_NAME__, __LINE__, "File not found");
        return;
    }
    E9::InitializeContexts();
    size_t len1 = host->sourceFile.size();
    size_t len2 = std::string(".scheat").size();
    if (len1 >= len2 && host->sourceFile.compare(len1 - len2, len2, ".scheat") == 0){
        E9::CreateMainContext();
    }
    mTokens = host->tokens;
    p_unique(Statements) stmt = nullptr;
    while (stmt = parseStatements(mTokens),stmt != nullptr) {
        stmt->codegen(ScheatContext::local()->stream_body);
        stmt = parseStatements(mTokens);
    }
    for (auto i = ScheatContext::contextCenter.begin(); i != ScheatContext::contextCenter.end(); i = std::next(i)) {
        (*i)->dump(f);
        f << "\n";
    }
}

std::vector<std::unique_ptr<Statements>> Parse(_Scheat *obj){
    return {};
}
