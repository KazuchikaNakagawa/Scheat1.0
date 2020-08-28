//
//  Lexer.cpp
//  scheat
//
//  Created by かずちか on 2020/07/31.
//

#include "scheat.h"


using namespace scheat;
Token *Token::last(){
    Token *cpy = this;
    if (cpy == nullptr) {
        return nullptr;
    }
    while (cpy->next != nullptr) {
        cpy = cpy->next;
    }
    return cpy;
}

Token *Token::first(){
    Token *cpy = this;
    if (cpy == nullptr) {
        return nullptr;
    }
    
    while (cpy->prev != nullptr) {
        cpy = cpy->prev;
    }
    
    return cpy;
}

Lexer::Lexer(scheat::Scheat *host){
    buf = "";
    skipFlag = false;
    this->host = host;
    tokens = nullptr;
    commentDepth = 0;
    state = initState;
}

void Lexer::lex(std::ifstream &stream){
    if (!stream.is_open()) {
        host->FatalError(__FILE_NAME__, __LINE__, "input file is not open --Lexer");
    }
    int c;
    while (c = stream.get(), c != EOF) {
        input(c, stream.get());
        stream.unget();
    }
}

void Token::valInt(std::string k){
    value.intValue = atoi(k.c_str());
    kind = TokenKind::val_num;
}

void Token::valStr(std::string k){
    if (k[0] != '"') {
        printf("Scheat System Error %u\n", __LINE__);
        printf("    reason: string started \" else character.\n");
        exit(0);
    }
    // not sure if it doesn't make crash
    if (k[k.length() - 1] != '"') {
        printf("Scheat System Error %u\n", __LINE__);
        printf("    reason: string ended \" else character.\n");
        exit(0);
    }
    value.strValue = k;
    kind = TokenKind::val_str;
}

void Token::valBool(std::string k){
    if (k == "true") {
        value.boolValue = true;
    }else if (k == "false"){
        value.boolValue = false;
    }else if (k == "Yes"){
        value.boolValue = true;
    }else if (k == "No"){
        value.boolValue = false;
    }else{
        printf("Scheat System Error %u\n", __LINE__);
        printf("    reason: illegal bool token error\n");
        exit(0);
    }
    kind = TokenKind::val_bool;
}

void Token::valDouble(std::string k){
    value.doubleValue = atof(k.c_str());
    kind = TokenKind::val_double;
}

Token *Token::add(Token *tokens, Token *token){
    if (tokens == nullptr) {
        return token;
    }
    tokens->next = token;
    token->prev = tokens;
    tokens = tokens->next;
    return tokens;
}

#define tadd tokens = Token::add(tokens, tok)

void Lexer::genTok(){
    //printf("genTok with %s\n", buf.c_str());
    if (buf.empty()) {
        state = initState;
        return;
    }
    Token *tok = new Token();
    if (buf == "...") {
        tok->kind = TokenKind::val_operator;
        tok->value.strValue = buf;
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (buf == ".") {
        tok->kind = TokenKind::tok_period;
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (buf == ",") {
        tok->kind = TokenKind::tok_comma;
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (buf == "(") {
        tok->kind = TokenKind::tok_paren_l;
        tadd;
        clear();
        return;
    }
    if (buf == ")") {
        tok->kind = TokenKind::tok_paren_r;
        tadd;
        clear();
        return;
    }
    if (state == numberState) {
        tok->valInt(buf);
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (state == stringState) {
        tok->valStr(buf);
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (state == doubleState) {
        tok->valDouble(buf);
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (state == operatorState) {
        tok->value.strValue = buf;
        tok->kind = TokenKind::val_operator;
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (buf == "this" || buf == "new" || buf == "these") {
        tok->kind = TokenKind::tok_this;
        tadd;
        clear();
        return;
    }
    
    if (buf == "is" || buf == "are") {
        tok->kind = TokenKind::tok_is;
        tadd;
        clear();
        return;
    }
    
    if (buf == "that" || buf == "the" || buf == "those") {
        tok->kind = TokenKind::tok_that;
        tadd;
        clear();
        return;
    }
    
    if (buf == "remember") {
        tok->kind = TokenKind::tok_external;
        tadd;
        clear();
        return;
    }
    
    if (buf == "print") {
        tok->kind = TokenKind::embbed_func_print;
        tadd;
        clear();
        return;
    }
    
    if (buf == "assemble") {
        tok->kind = TokenKind::embbed_func_assemble;
        tadd;
        clear();
        return;
    }
    
    if (state == identifierState) {
        tok->value.strValue = buf;
        tok->kind = TokenKind::val_identifier;
        tadd;
    }
    clear();
}

#undef tadd

void Token::out(){
    if (kind == TokenKind::tok_range) {
        printf("... token\n");
    }
    if (kind == TokenKind::val_num) {
        printf("integer token ->%d\n", value.intValue);
        return;
    }
    if (kind == TokenKind::val_str) {
        printf("string token ->%s\n", value.strValue.c_str());
        return;
    }
    if (kind == TokenKind::val_operator) {
        printf("operator token ->%s\n", value.strValue.c_str());
        return;
    }
    if (kind == TokenKind::val_double) {
        printf("float-point token ->%lf\n", value.doubleValue);
        return;
    }
    if (kind == TokenKind::val_bool) {
        if (value.boolValue) {
            printf("bool token ->true\n");
        }else{
            printf("bool token ->false\n");
        }
        return;
    }
    if (kind == TokenKind::val_identifier) {
        printf("id token ->%s\n", value.strValue.c_str());
        return;
    }
    if (kind == TokenKind::tok_this) {
        printf("this token\n");
        return;
    }
    if (kind == TokenKind::tok_is) {
        printf("is token\n");
        return;
    }
    if (kind == TokenKind::tok_that) {
        printf("that token\n");
        return;
    }
    if (kind == TokenKind::tok_external) {
        printf("remember token\n");
        return;
    }
    if (kind == TokenKind::tok_period) {
        printf(". token\n");
    }
    if (kind == TokenKind::tok_comma) {
        printf(", token\n");
    }
    if (kind == TokenKind::tok_paren_l) {
        printf("( token\n");
    }
    if (kind == TokenKind::tok_paren_r) {
        printf(") token\n");
    }
    if (kind == TokenKind::embbed_func_print) {
        printf("print token\n");
    }
}

void Lexer::clear(){
    state = initState;
    buf = "";
    isPossibleForPPPTok = false;
}

void Lexer::input(int c, int next){
    host->DevLog(__FILE_NAME__,__LINE__, "%c was input, %s : now buffer", c, buf.c_str());
    if (c == '\0' || c == EOF) {
        return;
    }
    
    location.column++;
    
    if (c == '\n') {
        location.line++;
    }
    
    if (skipFlag) {
        skipFlag = !skipFlag;
        return;
    }
    
    if (commentDepth > 0 && c == '*' && next == '#' && state != stringState) {
        commentDepth--;
        skipFlag = true;
        if (commentDepth == 0) {
            clear();
        }
        return;
    }
    
    if (commentDepth > 0) {
        return;
    }
    
    if (true
        && state == commentState
        && c != '\n') {
        
        return;
        
    }else if (state == commentState && c == '\n'){
        clear();
        return;
    }
    
    if (c == '"' && state == stringState) {
        buf.push_back(c);
        genTok();
        return;
    }
    
    if (c == '"') {
        genTok();
        buf.push_back(c);
        state = stringState;
        return;
    }
    
    if (state == stringState && c == '\\') {
        if (next == 'n') {
            buf.push_back('\n');
            skipFlag = true;
            return;
        }
        if (next == 'b') {
            buf.push_back('\b');
            skipFlag = true;
            return;
        }
        if (next == '0') {
            buf.push_back('\0');
            skipFlag = true;
            return;
        }
        buf.push_back(next);
        skipFlag = true;
        return;
    }
    
    if (state == stringState) {
        buf.push_back(c);
        return;
    }
    
    if (c == '\n') {
        genTok();
        return;
    }
    
    if (c == ' ') {
        genTok();
        return;
    }
    
    if (c == '\t') {
        genTok();
        return;
    }
    
    if (c == '#' && next == '*') {
        genTok();
        state = longCommentState;
        commentDepth++;
    }
    
    if (c == '#') {
        state = commentState;
        return;
    }
    
    if (isalpha(c) || c == '_') {
        if (state == operatorState) {
            genTok();
            buf.push_back(c);
            state = identifierState;
            return;
        }
        if (state == initState) {
            state = identifierState;
            buf.push_back(c);
            return;
        }
        if (state == identifierState) {
            buf.push_back(c);
            return;
        }
        host->FatalError(__FILE_NAME__,__LINE__, "in file %d.%d illegal character %c was input.", location.line, location.column, c);
        return;
    }
    
    if (isdigit(c)) {
        if (state == operatorState) {
            genTok();
            buf.push_back(c);
            state = numberState;
            return;
        }
        if (state == numberState) {
            buf.push_back(c);
            return;
        }
        if (state == identifierState) {
            buf.push_back(c);
            return;
        }
        if (state == initState) {
            buf.push_back(c);
            state = numberState;
            return;
        }
        if (state == doubleState) {
            buf.push_back(c);
            state = doubleState;
            return;
        }
        host->FatalError(__FILE_NAME__,__LINE__, "in file %d.%d illegal character %c was input.", location.line, location.column, c);
        return;
    }
    
    if (c == '.') {
        if (state == numberState && isdigit(next)) {
            state = doubleState;
            buf.push_back(c);
            return;
        }
        
        if (next == '.' && isPossibleForPPPTok) {
            // .. <- .
            buf.push_back(c);
            buf.push_back(next);
            genTok();
            skipFlag = true;
            return;
        }
        else if (next == '.' && !isPossibleForPPPTok) {
            genTok();
            buf.push_back(c);
            isPossibleForPPPTok = true;
            return;
        }
        else if (isPossibleForPPPTok && next != '.'){
            // .. token
            host->FatalError(__FILE_NAME__, __LINE__, "invalid input '..' . Did you mean '...'?");
        }
        genTok();
        buf.push_back(c);
        genTok();
    }
    
    if (c == ',') {
        if (state == numberState && isdigit(next)) {
            return;
        }
        genTok();
        buf.push_back(c);
        genTok();
    }
    
    if (c == '(') {
        genTok();
        buf.push_back(c);
        genTok();
    }
    
    if (c == ')') {
        genTok();
        buf.push_back(c);
        genTok();
    }
    
    if (false
        || c == '+'
        || c == '-'
        || c == '='
        || c == '&'
        || c == '*'
        || c == '/'
        || c == '^'
        || c == '!'
        || c == '%'
        || c == '$') {
        if (state == initState) {
            state = operatorState;
            buf.push_back(c);
            return;
        }
        if (state == operatorState) {
            buf.push_back(c);
            return;
        }
        genTok();
        buf.push_back(c);
        state = operatorState;
        return;
    }

}

void Token::enumerate(){
    Token *a = this->first();
    while (a != nullptr) {
        a->out();
        a = a->next;
    }
}

void Lexer::lex(std::string str){
    int length = str.length();
    host->targettingFile = "User input";
    for (int i = 0; i < length; i++) {
        input(str[i], str[i + 1]);
        if (str[i+1] == '\0') {
            input(' ', ' ');
        }
    }
}

