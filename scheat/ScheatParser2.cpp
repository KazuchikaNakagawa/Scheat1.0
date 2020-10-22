//
//  YAScheatParser.cpp
//  scheat
//
//  Created by かずちか on 2020/09/26.
//

#include "ScheatParser2.h"

/*
 Parsing
 
 Statement
    1. check the keyword
    this -> parseDeclareVarStatement
    print -> parsePrintStatement
    to -> parseDeclareFuncStatement
    if -> parseIfStatement
    unless ->
    for -> parseForStatement
    while -> parseWhileStatement
    
    id_expr -> parseIdentifierExpressionStatement
 */

using namespace std;
using namespace scheat;
using namespace basics;
using namespace nodes2;
using namespace parser2;
using scheat::statics::contextCenter;
using scheat::statics::global_context;
using scheat::statics::main_Context;
using scheat::statics::local_context;
using scheat::statics::objects;
using scheat::statics::fname;
using scheat::statics::mTokens;
using scheat::statics::scheato;

static bool isValue(Token *tok){
    switch (tok->kind) {
        case scheat::TokenKind::val_num:
            return true;
            break;
        case scheat::TokenKind::val_operator:
            return true;
        case scheat::TokenKind::val_str:
            return true;
        case scheat::TokenKind::val_bool:
            return true;
        case scheat::TokenKind::val_double:
            return true;
        case scheat::TokenKind::val_identifier:
            return true;
        default:
            break;
    }
    return false;
}

static unique_ptr<IdentifierExpr> parseIdentifierExpr(Token *&tok){
    
    // identifierExpr : the ID
    //                | this
    //                | ID . ID
    if (tok->kind == scheat::TokenKind::tok_this) {
        
    }
    
    auto base = tok;
    Token *opTok = nullptr;
    if (tok->next->kind == scheat::TokenKind::tok_access) {
        
    }else{
        auto ptr = make_unique<IdentifierTerm>();
    }
    return nullptr;
}

static unique_ptr<Term> parseTermNodes(Token*& tok){
    if (tok->kind == scheat::TokenKind::val_num) {
        auto ptr = make_unique<IntTerm>(tok);
        return ptr;
    }
    if (tok->kind == scheat::TokenKind::val_str) {
        auto ptr = make_unique<StringTerm>(tok);
        return ptr;
    }
    if (tok->kind == scheat::TokenKind::val_bool) {
        auto ptr = make_unique<BoolTerm>(tok);
        return ptr;
    }
    if (tok->kind == scheat::TokenKind::val_double) {
        auto ptr = make_unique<FloatTerm>(tok);
        return ptr;
    }
    if (tok->kind == scheat::TokenKind::val_identifier) {
        
    }
    return nullptr;
}

extern unique_ptr<DeprecatedTerm> parser2::parseTerm(Token *&tok){
    
    return nullptr;
}

extern unique_ptr<OperatedPrimaryExpr> parser2::parsePrimary(Token *&tok){
    parsePrefix:
    // primary : OP primary
    if (tok->kind == scheat::TokenKind::val_operator) {
        Token *savedOP = tok;
        auto saved = tok->next;
        auto primary = parser2::parsePrimary(saved);
        if (!primary) {
            return nullptr;
        }
        if (scheato->hasProbrem()) {
            return nullptr;
        }
        auto type = global_context->findClass(primary->type.name);
        if (!type) {
            scheato->FatalError(primary->location,
                                __FILE_NAME__, __LINE__,
                                "in %d.%d Unknown type %s",
                                primary->location.line,
                                primary->location.column,
                                primary->type.name.c_str());
            return nullptr;
        }
        auto op = type->findOperator(savedOP->value.strValue);
        if (!op) {
            goto parseInfix;
        }else{
            tok = saved;
        }
        if (op->position != op->prefix) {
            scheato->FatalError(savedOP->location,
                                __FILE_NAME__, __LINE__,
                                "operator %s is not prefix operator",
                                savedOP->value.strValue.c_str());
            return nullptr;
        }
        if (primary->type != op->lhs_type) {
            scheato->FatalError(savedOP->location, __FILE_NAME__, __LINE__,
                                "operator %s(%s) is undefined",
                                savedOP->value.strValue.c_str(),
                                op->lhs_type->name.c_str());
            return nullptr;
        }
        
    }
    parseInfix:
    auto term = parseTerm(tok);
    if (!term) {
        return nullptr;
    }
    
    return nullptr;
}

extern unique_ptr<Expr> parser2::parseExpr(Token *&tok){
    unique_ptr<Expr> toreturn = nullptr;
    parsePrefix:
    // expr : op expr
    if (tok->kind == scheat::TokenKind::val_operator) {
        Token *saved = tok;
        Token *saved2 = tok->next;
        auto expr = parser2::parseExpr(saved2);
        if (!expr) {
            return nullptr;
        }
        if (!scheato->hasProbrem()) {
            tok = saved2;
        }
        auto type = global_context->findClass(expr->type.name);
        if (type == nullptr) {
            scheato->FatalError(expr->location,
                                __FILE_NAME__, __LINE__,
                                "in %d.%d Unknown type %s",
                                expr->location.line,
                                expr->location.column,
                                expr->type.name.c_str());
            return nullptr;
        }
        auto opiter = type->operators.find(saved->value.strValue);
        
        if (opiter == type->operators.end()) {
            // expr : op (expr) -> expr : primary(op expr)
            tok = saved;
            goto parseLhs;
        }
        
        if ((*opiter).second->precidence != scheat::basics::Operator::secondary) {
            // op (expr) -> (op expr)
            tok = saved;
            
            goto parseLhs;
        }
        
        return nullptr;
    }
    parseLhs:
    auto prim =  parser2::parsePrimary(tok);
    if (!prim) {
        return nullptr;
    }
    auto type = global_context->findClass(prim->type.name);
    if (!type) {
        scheato->FatalError(prim->location,
                            __FILE_NAME__, __LINE__,
                            "in %d.%d Unknown type %s",
                            prim->location.line,
                            prim->location.column,
                            prim->type.name.c_str());
    }
    Token *optok = nullptr;
    parsePostfix:
    if (tok->kind == scheat::TokenKind::val_operator) {
        // expr : primary op
        optok = tok;
        eatThis(tok);
        // check if infix or postfix
        if (isValue(tok)) {
            goto parseInfix;
        }
        auto oper = type->operators[optok->value.strValue];
        if (oper == nullptr) {
            scheato->FatalError(prim->location,
                                __FILE_NAME__, __LINE__,
                                "in %d.%d type %s has no operator %s",
                                prim->location.line,
                                prim->location.column,
                                prim->type.name.c_str(),
                                optok->value.strValue.c_str());
            return nullptr;
        }
        if (oper->position != scheat::basics::Operator::postfix) {
            scheato->FatalError(optok->location,
                                __FILE_NAME__, __LINE__,
                                "in %d.%d operator %s is not a infix operator",
                                optok->location.line,
                                optok->location.column,
                                optok->value.strValue.c_str());
        }
    }else{
        return prim;
    }
    parseInfix:
    auto expr = parser2::parseExpr(tok);
    auto typer = global_context->findClass(expr->type.name);
    auto oper = type->operators[optok->value.strValue];
    if (oper == nullptr) {
        scheato->FatalError(prim->location,
                            __FILE_NAME__, __LINE__,
                            "in %d.%d type %s has no operator %s",
                            prim->location.line,
                            prim->location.column,
                            prim->type.name.c_str(),
                            optok->value.strValue.c_str());
        return nullptr;
    }
    if (oper->position != scheat::basics::Operator::infix) {
        scheato->FatalError(optok->location,
                            __FILE_NAME__, __LINE__,
                            "in %d.%d operator %s is not a infix operator",
                            optok->location.line,
                            optok->location.column,
                            optok->value.strValue.c_str());
    }
    if (expr->type == oper->rhs_type) {
        scheato->FatalError(optok->location,
                            __FILE_NAME__, __LINE__,
                            "operator %s(the %s, %s) does not exists",
                            optok->value.strValue.c_str(),
                            prim->type.name.c_str(),
                            typer->context->name.c_str());
        return nullptr;
    }
    return InfixOperatorExpr::init(move(prim), oper, move(expr));

}

extern void parser2::parse(Scheat *sch,Token *tokens){
    scheato = sch;
    sch->statements = new DataHolder();
    auto stmts = make_unique<Statements>();
    stmts->statements = nullptr;
    stmts->statement = parseStatement(tokens);
    if (scheato->hasProbrem()) {
        return;
    }
    if (!stmts->statement) {
        return;
    }
    
    
    while (tokens == nullptr){
    
        auto s = parseStatement(tokens);
        if (!s && !scheato->hasProbrem()) {
            return;
        }
        
        if (scheato->hasProbrem()) {
            return;
        }
        
        stmts = make_unique<Statements>(move(stmts), move(s));
        
    }
    
    scheato->statements->statements = move(stmts);
    return;
};

extern unique_ptr<Statement> parser2::parseStatement(Token *&tokens){
    auto sts = make_unique<Statement>();
    sts -> statement = parseStatement_single(tokens);
    if (!sts->statement) {
        return nullptr;
    }
    unique_ptr<StatementNode> s = nullptr;
    while (s = parseStatement_single(tokens) , s != nullptr) {
        sts->statement = move(s);
        if (tokens->kind == TokenKind::tok_period) {
            sts->perTok = tokens;
            eatThis(tokens);
        }else if (tokens->kind == TokenKind::tok_comma){
            sts->perTok = tokens;
            eatThis(tokens);
        }else{
            return nullptr;
        }
        sts = make_unique<Statement>(move(sts));
    }
    return nullptr;
}

extern unique_ptr<StatementNode> parser2::parseStatement_single(Token *&tokens){
    return nullptr;
}
