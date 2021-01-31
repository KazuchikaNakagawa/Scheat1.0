//
//  YAScheatParser.cpp
//  scheat
//
//  Created by かずちか on 2020/09/26.
//

#include "ScheatParser2.h"
#include <fstream>
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
using namespace nodes2;
using namespace parser2;

unique_ptr<StatementNode> parseStatement_single(Token *&);

static bool isValue(Token *tok){
    if (tok == nullptr) {
        return false;
    }
    switch (tok->kind) {
        case scheat::TokenKind::val_num:
            return true;
            break;
        case scheat::TokenKind::val_operator:
            return false;
        case scheat::TokenKind::val_str:
            return true;
        case scheat::TokenKind::val_bool:
            return true;
        case scheat::TokenKind::val_double:
            return true;
        case scheat::TokenKind::val_identifier:
            return true;
        case scheat::TokenKind::tok_paren_l:
            return true;
        default:
            break;
    }
    return false;
}

static Operator *findOperator(Token *tok, TypeData type, OperatorPosition position, OperatorPresidence priority){
    auto cla = ScheatContext::global->findClass(type.name);
    if (!cla) {
        return nullptr;
    }
    auto iter = cla->operators.find(tok->value.strValue);
    if (iter == cla->operators.end()) {
        return nullptr;
    }
    
    
    if (iter->second->precidence != priority) {
        return nullptr;
    }
    
    
    if (iter->second->position != position) {
        return nullptr;
    }
    
    
    return iter->second;
}

//extern unique_ptr<IdentifierExprTemplate> parseIdentifierExpr(Token *&);
static unique_ptr<ArgumentExpr> parseArgumentExpr(Token*& tok);

static unique_ptr<IdentifierTerm> parseIdentifierTerm(TypeData parentType,Token *&tok){
    
    if (tok->kind == scheat::TokenKind::val_identifier){
        auto idtok = tok;
        eatThis(tok);
        if (tok->kind == scheat::TokenKind::tok_paren_l) {
            eatThis(tok);
            //
            auto classptr = ScheatContext::global->findClass(parentType.name);
            if (!classptr) {
                scheato->FatalError(tok->location, __FILE_NAME__, __LINE__, "type %s is undefined.", parentType.name.c_str());
                return nullptr;
            }
            auto attptr = classptr->context->findFunc(idtok->value.strValue);
            if (!attptr) {
                scheato->FatalError(idtok->location, __FILE_NAME__, __LINE__, "%s's %s function is undefined.",
                                    parentType.name.c_str(),
                                    idtok->value.strValue.c_str());
                return nullptr;
            }
            auto ptr = parseArgumentExpr(tok);
            if (!ptr) {
                //return nullptr;
                return FunctionAttributeExpr::init(attptr, move(ptr), idtok->location);
            }
            if (tok->kind != scheat::TokenKind::tok_paren_r) {
                scheato->FatalError(tok->location, __FILE_NAME__, __LINE__, "there are no right parentheses.");
                return nullptr;
            }
            eatThis(tok);
            return FunctionAttributeExpr::init(attptr, move(ptr), idtok->location);
            
        }else if (tok->kind == scheat::TokenKind::tok_with){
            eatThis(tok);
            if (tok->kind != scheat::TokenKind::tok_paren_l) {
                scheato->FatalError(tok->location, __FILE_NAME__, __LINE__, "( is needed after with keyword.");
                return nullptr;
            }
            eatThis(tok);

        }else{
            auto classptr = ScheatContext::global->findClass(parentType.name);
            if (!classptr) {
                scheato->FatalError(idtok->location, __FILE_NAME__, __LINE__,
                                    "type %s is undefined. you may forget to import external files.",
                                    parentType.name.c_str());
                return nullptr;
            }
            auto varindex = classptr->properties.find(idtok->value.strValue);
            if (varindex == classptr->properties.end()) {
                scheato->FatalError(idtok->location, __FILE_NAME__, __LINE__, "%s has no property named %s",
                                    parentType.name.c_str(),
                                    idtok->value.strValue.c_str());
                return nullptr;
            }
//            auto varptr = classptr->context->findVariable(idtok->value.strValue);
//            if (!varptr) {
//                scheato->FatalError(idtok->location, __FILE_NAME__, __LINE__, "%s has no property named %s",
//                                    parentType.name.c_str(),
//                                    idtok->value.strValue.c_str());
//                return nullptr;
//            }
            auto ptr = VariableAttributeExpr::init(varindex->second, idtok->location);
            return ptr;
        }
    }
    return nullptr;
}

int hasProperty(TypeData type, string k){
    Class *cl = ScheatContext::global->findClass(type.name);
    if (!cl) {
        scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__, "-? type %s does not exist...", type.name.c_str());
        return 0;
    }
    auto itervar = cl->properties.find(k);
    if (itervar == cl->properties.end()) {
        scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__, "-? property %s does not exist...", k.c_str());
        return 0;
    }else{
        return itervar->second.index;
    }
    return 0;
}

static unique_ptr<IdentifierExpr> parseFirstIdentifierExpr(Token *&tok){
    if (tok->kind != scheat::TokenKind::val_identifier) {
        tok->out();
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__, "it is not suitable for expression. you may solve this by writing a parentheses of arguments.");
        return nullptr;
    }
    auto idtok = tok;
    eatThis(tok);
    if (tok->kind == scheat::TokenKind::tok_with) {
        eatThis(tok);
    }
    if (tok->kind == scheat::TokenKind::tok_paren_l) {
        eatThis(tok);
        auto func = ScheatContext::global->findFunc(idtok->value.strValue);
        auto funcnode = FunctionCallTerm::init(idtok, func);
        if (!func) {
            scheato->FatalError(idtok->location, __FILE_NAME__, __LINE__, "%s is undefined. Make sure if it is function.", idtok->value.strValue.c_str());
            return nullptr;
        }
        while (tok->kind != scheat::TokenKind::tok_paren_r) {
            auto ptr = parseExpr(tok);
            if (!ptr) {
                return nullptr;
            }
            funcnode->args.push_back(move(ptr));
            if (tok->kind == scheat::TokenKind::tok_comma) {
                eatThis(tok);
            }else{
                break;
            }
        }
        eatThis(tok);
        return funcnode;
    }else{
        auto var = ScheatContext::local()->findVariable(idtok->value.strValue);
        if (!var) {
            scheato->FatalError(idtok->location, __FILE_NAME__, __LINE__, "%s is undefined.",
                                idtok->value.strValue.c_str());
            return nullptr;
        }
        auto varnode = VariableTerm::init(idtok, var->type);
        return varnode;
    }
    return nullptr;
}

unique_ptr<IdentifierExpr> parseIdentifierExpr(Token *&tok){
//
    // identifierExpr : the ID
    //                | this ID
    //                | ID . ID
    if (tok->kind == scheat::TokenKind::tok_the) {
        auto saved = tok;
        eatThis(tok);
        auto ptr = parseIdentifierExpr(tok);
        if (!ptr) {
            scheato->FatalError(tok->location, __FILE_NAME__, __LINE__, "identifier after 'the' is unclear.");
            return nullptr;
        }
        auto ret = TheIdentifierExpr::init(saved, move(ptr));
        return ret;
    }
//
    auto ptr = parseFirstIdentifierExpr(tok);
    if (!ptr) {
        return nullptr;
    }
    while (true){
        if (tok->kind == scheat::TokenKind::val_identifier) {
            auto ch = parseIdentifierTerm(ptr->type, tok);
            if (!ch) {
                return nullptr;
            }
            ptr = AccessIdentifierExpr::init(move(ptr), move(ch));
            
        }else{
            if (tok->kind == scheat::TokenKind::tok_access) {
                eatThis(tok);
                continue;
            }
            break;
        }
    }
    return ptr;
}

static TypeData *parseTypeExpr(Token *&tok){
    if (tok->kind == scheat::TokenKind::tok_of) {
        // generics
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
                            "generics are not available on Scheat 2.0");
        return nullptr;
    }
    if (tok->kind == scheat::TokenKind::tok_the) {
        eatThis(tok);
        auto tp = parseTypeExpr(tok);
        if (!tp) {
            return nullptr;
        }
        return new TypeData("the " + tp->name, tp->ir_used + "*");
    }
    if (tok->kind == scheat::TokenKind::val_identifier) {
        auto classptr = ScheatContext::global->findClass(tok->value.strValue);
        if (!classptr) {
            scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
                                "name '%s' does not exist",
                                tok->value.strValue.c_str());
            return nullptr;
        }
        return classptr->type;
    }
    scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
                        "type expression is expected but nowhere to be found");
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
        auto ptr = parseIdentifierExpr(tok);
    }
    return nullptr;
}

static unique_ptr<ArgumentExpr> parseArgumentExpr(Token *&tok){
    if (tok->kind == scheat::TokenKind::tok_paren_r) {
        return ArgumentExpr::init(nullptr);
    }
    auto first = parseExpr(tok);
    if (!first) {
        return nullptr;
    }
    auto ptr = ArgumentExpr::init(move(first));
    if (tok->kind != scheat::TokenKind::tok_comma) {
        return ptr;
    }
    eatThis(tok);
    while (true) {
        auto expr = parseExpr(tok);
        if (!expr) {
            return nullptr;
        }
        ptr = ArgumentExpr::addArg(move(ptr), move(expr));
        if (tok->kind == scheat::TokenKind::tok_comma) {
            eatThis(tok);
        }else{
            break;
        }
    }
    return ptr;
}


extern unique_ptr<Term> scheat::parser2::parseTerm(Token *&tok){
    // term : identifierExpr
    //      | int
    //      | string
    //      | bool
    //      | float
    //      | PrifixOperatedTerm
    //      | ...
    
    // prefix
    //-------------------------------------------------------------------------
    if (tok->kind == TokenKind::val_operator) {
        auto saved = tok;
        eatThis(tok);
        auto term = parseTerm(tok);
        if (!term) {
            return nullptr;
        }
        
        auto op = findOperator(saved, term->type, prefix, termly);
        if (!op) {
            scheato->FatalError(saved->location, __FILE_NAME__, __LINE__,
                                "%s was not found in class %s",
                                saved->value.strValue.c_str(),
                                term->type.name.c_str());
            return nullptr;
        }
        
        auto operTerm = PrefixOperatorTerm::init(op, move(term));
        return operTerm;
    }
    
    if (tok->kind == TokenKind::tok_paren_l) {
        eatThis(tok);
        auto expr = parseExpr(tok);
        if (!expr) {
            return nullptr;
        }
        if (tok->kind != TokenKind::tok_paren_r) {
            scheato->FatalError(tok->location, __FILE_NAME__, __LINE__, "There must be ) after value.");
            return nullptr;
        }
        eatThis(tok);
        return ParenthesesExpr::init(move(expr));
    }
    
    unique_ptr<Term> ptr = nullptr;
    
    // immediates
    if (tok->kind == TokenKind::val_num) {
        ptr = make_unique<IntTerm>(tok);
        eatThis(tok);
    }
    
    else if (tok->kind == TokenKind::val_str) {
        ptr = make_unique<StringTerm>(tok);
        eatThis(tok);
    }
    
    else if (tok->kind == TokenKind::val_bool) {
        ptr = make_unique<BoolTerm>(tok);
        eatThis(tok);
    }
    
    else if (tok->kind == TokenKind::val_double) {
        ptr = make_unique<FloatTerm>(tok);
        eatThis(tok);
    }
    
    else if (tok->kind == TokenKind::val_identifier || tok->kind == TokenKind::tok_the) {
        ptr = parseIdentifierExpr(tok);
        //eatThis(tok);
    }
    
    else{
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__, "Regular value was not found.");
        return nullptr;
    }
    
    // -------------------------------------------------------------------------
    
    // infix/postfix
    
    if (tok == nullptr) {
        return ptr;
    }
    
    if (tok->kind == TokenKind::val_operator) {
        
        if (isValue(tok->next)) {
            goto ParseInfix;
        }
        
        auto op = findOperator(tok, ptr->type, postfix, termly);
        if (!op) {
//            scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
//                                "%s was not found in class %s",
//                                tok->value.strValue.c_str(),
//                                ptr->type.name.c_str());
            return ptr;
        }
        eatThis(tok);
        auto operatedTerm = PostfixOperatorTerm::init(move(ptr), op);
        return operatedTerm;
    }
    
    ParseInfix:
    auto op = findOperator(tok, ptr->type, infix, termly);
    if (!op) {
        return ptr;
    }
    eatThis(tok);
    auto rhs = parsePrimary(tok);
    if (!rhs) {
        return nullptr;
    }
    return nullptr;
}

static unique_ptr<PrimaryExpr> parseOperatedPrimaryExpr(Token *&tok){
    // primary : term
    //         | term op primary
    //         | op primary
    unique_ptr<Term> ptr = nullptr;
    if (tok->kind == TokenKind::val_operator) {
        auto saved = tok;
        eatThis(tok);
        auto term = parsePrimary(tok);
        auto op = findOperator(tok, term->type, prefix, primary);
        if (!op) {
            tok = saved;
            goto infix_postfix;
        }
        auto ptr = PrefixOperatorPrimaryExpr::init(op, move(term));
    }
    
infix_postfix:
    ptr = parseTerm(tok);
    
    if (!ptr) {
        return nullptr;
    }
    
    if (!tok) {
        return ptr;
    }
    
    if (tok->kind == TokenKind::val_operator) {
        if (isValue(tok->next)) {
            // infix
            auto saved = tok;
            eatThis(tok);
            auto primaryptr = parsePrimary(tok);
            if (!primaryptr) {
                return nullptr;
            }
            auto op = findOperator(saved, ptr->type, infix, primary);
            if (!op) {
                tok = saved;
                return ptr;
            }
            
            auto ret = InfixOperatorPrimaryExpr::init(move(ptr), op, move(primaryptr));
            return ret;
            
        }else{
            // postfix
            auto op = findOperator(tok, ptr->type, postfix, primary);
            if (!op) {
                return ptr;
            }
            eatThis(tok);
            auto ret = PostfixOperatorPrimaryExpr::init(move(ptr), op);
            return ret;
        }
        
    }
    
    
    return ptr;
}

extern unique_ptr<PrimaryExpr> scheat::parser2::parsePrimary(Token *&tok){
    // primary : term
    //         | term op primary
    //         | op primary
    return parseOperatedPrimaryExpr(tok);
}

static unique_ptr<Expr> parseOperatedExpr(Token *&tok){
    if (tok == nullptr) {
        return nullptr;
    }
    // 1. op expr
    unique_ptr<PrimaryExpr> ptr = nullptr;
    
    if (tok->kind == TokenKind::val_operator) {
        auto saved = tok;
        eatThis(tok);
        auto rhs = parseExpr(tok);
        if (!rhs) {
            return nullptr;
        }
        auto op = findOperator(saved, rhs->type, prefix, secondary);
        if (!op) {
            // (!primary)
            tok = saved;
            goto infix_postfix;
        }
        
        auto ret = PrefixOperatorExpr::init(op, move(rhs));
        return ret;
    }
    
infix_postfix:
    auto prim = parsePrimary(tok);
    
    if (!prim) {
        return nullptr;
    }
    
    if (!tok) {
        return prim;
    }
    
    if (tok->kind == TokenKind::val_operator) {
        
        if (isValue(tok->next)) {
            auto saved = tok;
            eatThis(tok);
            auto expr = parseExpr(tok);
            if (!expr) {
                return nullptr;
            }
            auto op = findOperator(saved, expr->type, infix, secondary);
            if (!op) {
                scheato->FatalError(saved->location, __FILE_NAME__, __LINE__,
                                    "%s was not found in class %s",
                                    saved->value.strValue.c_str(),
                                    expr->type.name.c_str());
                return nullptr;
            }
            auto ret = InfixOperatorExpr::init(move(prim), op, move(expr));
            return ret;
        }else{
            auto saved = tok;
            auto op = findOperator(saved, prim->type, postfix, secondary);
            if (!op) {
                scheato->FatalError(saved->location, __FILE_NAME__, __LINE__,
                                    "%s was not found in class %s",
                                    saved->value.strValue.c_str(),
                                    prim->type.name.c_str());
                return nullptr;
            }
            auto ret = PostfixOperatorExpr::init(move(prim), op);
        }
        
    }else{
        return prim;
    }
    
    return nullptr;
}

static unique_ptr<LoadExpr> parseLoadExpr(Token *&tok){
    eatThis(tok);
    auto p = parseExpr(tok);
    if (!p) {
        return nullptr;
    }
    return LoadExpr::init(p->type.loaded(), move(p));
}

extern unique_ptr<Expr> scheat::parser2::parseExpr(Token* &tok) {
    // expr : operatedExpr t_of id
    //      | the expr
    if (tok->kind == TokenKind::tok_loaded) {
        return parseLoadExpr(tok);
    }
    auto ptr = parseOperatedExpr(tok);
    if (!ptr) {
        return nullptr;
    }
    
    // expr of Type
    if (tok->kind == TokenKind::tok_of) {
        eatThis(tok);
        auto type = parseTypeExpr(tok);
        if (!type) {
            return nullptr;
        }
        return CastExpr::init(*type, move(ptr));
    }
    
    return ptr;
}



extern void parser2::parse(_Scheat *sch,Token *tokens){
    scheato = sch;
    //sch->statements = new DataHolder();
    auto stmts = make_unique<Statements>();
    stmts->statements = nullptr;
    stmts->statement = parseStatement(tokens);
    if (scheato->hasProbrem()) {
        return;
    }
    if (!stmts->statement) {
        return;
    }
    
    while (tokens != nullptr){
    
        auto s = parseStatement(tokens);
        if (!s && !scheato->hasProbrem()) {
            break;
        }
        
        if (scheato->hasProbrem()) {
            return;
        }
        
        stmts = make_unique<Statements>(move(stmts), move(s));
        
    }
    //auto stream = ScheatContext::local();
    stmts->codegen(ScheatContext::local()->stream_body);
    return;
};

static unique_ptr<NewIdentifierExpr> parseNewIdentifierExpr(Token *& tok){
    unique_ptr<NewIdentifierExpr> ptr = nullptr;
    bool glbl = false;
    if (tok->kind == scheat::TokenKind::tok_this) {
        eatThis(tok);
    }
    if (ScheatContext::local()->name == "main") {
        glbl = true;
    }
    if (ScheatContext::local()->name == "global") {
        glbl = true;
    }
    if (tok->kind == scheat::TokenKind::tok_global) {
        glbl = true;
        eatThis(tok);
    }
    else if (tok->kind == scheat::TokenKind::tok_local) {
        glbl = false;
        eatThis(tok);
    }
    if (tok->kind == scheat::TokenKind::val_identifier) {
        ptr = NewIdentifierExpr::init(tok->location, /*ScheatContext::getNamespace() + "_" +*/ tok->value.strValue, nullptr);
        eatThis(tok);
    }else{
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
                            "identifier must be defined like: this (global/local) (public/private) identifier");
    }
    if (tok->kind == scheat::TokenKind::tok_of) {
        eatThis(tok);
        auto type = parseTypeExpr(tok);
        if (!type) {
            return nullptr;
        }
        ptr->type = type;
    }
    ptr->isGlobal = glbl;
    return ptr;
}

extern unique_ptr<Statement> parser2::parseStatement(Token *&tokens){
    auto sts = make_unique<Statement>();
    sts->stmt = nullptr;
    sts -> statement = parseStatement_single(tokens);
    if (!sts->statement) {
        return nullptr;
    }
    if (tokens->kind == TokenKind::tok_period) {
        eatThis(tokens);
        return sts;
    }
    if (tokens->kind != TokenKind::tok_period && tokens->kind != TokenKind::tok_comma) {
        scheato->FatalError(tokens->location, __FILE_NAME__, __LINE__,
                            "statements must end with period.");
        return nullptr;
    }
    eatThis(tokens);
    auto j = move(sts->statement);
    sts = Statement::init(move(sts), move(j));
    unique_ptr<StatementNode> s = nullptr;
    while (s = parseStatement_single(tokens) , s != nullptr) {
        sts = Statement::init(move(sts), move(s));
        if (tokens->kind == TokenKind::tok_period) {
            sts->perTok = tokens;
            eatThis(tokens);
            return sts;
        }else if (tokens->kind == TokenKind::tok_comma){
            sts->perTok = tokens;
            eatThis(tokens);
        }else{
            scheato->FatalError(tokens->location, __FILE_NAME__, __LINE__, "the end of sentence must be period or comma in Scheat.");
            return nullptr;
        }
        
    }
    return sts;
}

static unique_ptr<StatementNode> parseFunctionCallStatement(Token *&tok){
    
    return nullptr;
}

static unique_ptr<PrintStatement> parsePrintStatement(Token *&tok){
    eatThis(tok);
    if (tok->kind == scheat::TokenKind::tok_with) {
        eatThis(tok);
    }
    if (tok->kind == scheat::TokenKind::tok_paren_l) {
        eatThis(tok);
    }else{
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
                            "there must be a '(' after 'print'");
        return nullptr;
    }
    auto ptr = parseArgumentExpr(tok);
    if (tok == nullptr) {
        scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__, "file ends illegal token.");
        return nullptr;
    }
    if (tok->kind == scheat::TokenKind::tok_paren_r) {
        eatThis(tok);
    }else{
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
                            "parentheses are not closed.");
        return nullptr;
    }
    if (!ptr) {
        return nullptr;
    }
    return PrintStatement::init(move(ptr));
}

static unique_ptr<IfStatement>
parseIfStatement(Token *&tok){
    eatThis(tok);
    auto expr = parseExpr(tok);
    if (!expr) {
        return nullptr;
    }
    if (expr->type.ir_used != "i1") {
        scheato->FatalError(expr->location, __FILE_NAME__, __LINE__, "no boolean expression after if");
        return nullptr;
    }
    if (tok->kind != scheat::TokenKind::tok_comma) {
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__, "no comma after if clause.");
        return nullptr;
    }else{
        eatThis(tok);
    }
    auto s = parseStatement(tok);
    if (!s) {
        return nullptr;
    }
    tok = tok->prev;
    tok->kind = scheat::TokenKind::tok_comma;
    return IfStatement::init(move(expr), move(s), nullptr);
}

static unique_ptr<ForStatement>
parseForStatement(Token *&tok){
    eatThis(tok);
    auto i = parseExpr(tok);
    if (!i) {
        return nullptr;
    }
    if (i->type.ir_used != "i32") {
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
                            "for statement needs integer value after 'for' keyword.");
        return nullptr;
    }
    if (tok->kind != scheat::TokenKind::tok_times) {
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
                            "for statement needs 'times' keyword after integer value.");
        return nullptr;
    }
    eatThis(tok);
    if (tok->kind != scheat::TokenKind::tok_comma) {
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
                            "for statement needs ',' after 'times' keyword.");
        return nullptr;
    }
    eatThis(tok);
    auto ss = parseStatement(tok);
    if (!ss) {
        return nullptr;
    }
    tok = tok->prev;
    return ForStatement::init(move(i), move(ss));
}

static unique_ptr<DeclareVariableStatement> parseDeclareVariableStatement(Token *&tok){
    //eatThis(tok);
    if (tok->kind != scheat::TokenKind::val_identifier) {
        //exit(0);
    }
    
    
    auto name = parseNewIdentifierExpr(tok);
    
    if (!name) {
        return nullptr;
    }
    string name_raw = name->value;
    if (tok->kind != scheat::TokenKind::tok_is) {
        scheato->FatalError(tok->location, __FILE_NAME__, __LINE__,
                            "to define variable, needs 'is' after name.");
        return nullptr;
    }
    eatThis(tok);
    
    auto val = parseExpr(tok);
    if (!val) {
        scheato->FatalError(name->location, __FILE_NAME__, __LINE__, "to declare a variable, there must be a value after 'is' keyword.");
        return nullptr;
    }
    string prefix = "%";
    if ((ScheatContext::local()->name == "main"
        || ScheatContext::local()->name == "global")) {
        prefix = "@";
        if (!name->isGlobal) {
            prefix = "%";
        }else{
            name->isGlobal = true;
        }
    }else{
        if (name->isGlobal) {
            prefix = "@";
        }else{
            name->isGlobal = false;
        }
    }
    string kname;
    if (name->isGlobal) {
        kname = prefix + "glbl_" + name->value;
    }else{
        kname = prefix + ScheatContext::getNamespace() + "_" +  name->value;
    }
    auto var = new Variable(kname, val->type);
    
    if (name->isGlobal) {
        ScheatContext::global->addVariable(name_raw, var);
    }else{
        ScheatContext::local()->addVariable(name_raw, var);
    }
    
    if (!name->type) {
        name->type = &val->type;
    }
    
    auto stmtptr = DeclareVariableStatement::init(move(name), move(val));
    if (prefix == "@") {
        stmtptr->isGlobal = true;
    }
    stmtptr->name = var->mangledName;
    
    return stmtptr;
}

static unique_ptr<ReassignStatement>
parseReassignStatement(Token *&tok){
    
    //printf("yobareteru\n");
    auto idexpr = parseIdentifierExpr(tok);
    if (!idexpr) {
        //printf("koko akasii?");
        return nullptr;
    }
    
    if (tok->kind != scheat::TokenKind::tok_is) {
        scheato->FatalError(tok->location,
                            __FILE_NAME__,
                            __LINE__,
                            "reassign statement needs 'is' keyword between name and value.");
        return nullptr;
    }else{
        eatThis(tok);
    }
    
    auto ltype = idexpr->type;
    auto value = parseExpr(tok);
    if (!value) {
        return nullptr;
    }
    auto rtype = value->type;
    //cout << "l ha " << ltype.name << " nandakedo r ha "<< rtype.name << " dayo\n";
    if (ltype == rtype.pointer()) {
        return ReassignStatement::init(move(idexpr), move(value));
    }else{
        scheato->FatalError(idexpr->location, __FILE_NAME__, __LINE__,
                            "variable %s is defined as %s type but assigned %s type.", idexpr->userdump().c_str(),
                            ltype.name.c_str(),
                            rtype.name.c_str());
        return nullptr;
    }
    return nullptr;
}

bool isIncluded(TokenKind k, Token *tokens){
    auto tk = tokens;
    while (tk->kind != scheat::TokenKind::tok_EOF) {
        if (tk->kind == k) {
            return true;
        }
        if (tk->kind == scheat::TokenKind::tok_period) {
            return false;
        }
        tk = tk->next;
        
    }
    return false;
}

extern unique_ptr<StatementNode> parseStatement_single(Token *&tokens){
    // statement : this id is expr.
    if (tokens == nullptr) {
        return nullptr;
    }
    if (tokens->kind == scheat::TokenKind::tok_EOF) {
        return nullptr;
    }
    if (tokens->kind == TokenKind::tok_this) {
        return parseDeclareVariableStatement(tokens);
    }
    if (tokens->kind == scheat::TokenKind::val_identifier) {
        // return parseFunctionCallStatement(tokens);
    }
    if (tokens->kind == scheat::TokenKind::embbed_func_print) {
        return parsePrintStatement(tokens);
    }
    if (tokens->kind == scheat::TokenKind::tok_if) {
        return parseIfStatement(tokens);
    }
    if (tokens->kind == scheat::TokenKind::tok_for) {
        return parseForStatement(tokens);
    }
    
    if (tokens->kind == scheat::TokenKind::tok_do) {
        eatThis(tokens);
        if (tokens->kind == scheat::TokenKind::tok_for) {
            return parseForStatement(tokens);
        }
    }
    
    
    
    
    if (isIncluded(scheat::TokenKind::tok_is, tokens)) {
        return parseReassignStatement(tokens);
    }
    
    scheato->FatalError(tokens->location, __FILE_NAME__, __LINE__,
                        "Syntax Error: Invalid syntax.");
    return nullptr;
}
