//
//  Lexer.cpp
//  scheat
//
//  Created by かずちか on 2020/07/31.
//

#include "Lexer.hpp"

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
    commentDepth = 0;
}

void Lexer::lex(std::ifstream stream){
    if (!stream.is_open()) {
        host->FatalError("input file is not open --Lexer", __LINE__);
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
        printf("    reason: bool error\n");
        exit(0);
    }
    kind = TokenKind::val_bool;
}

void Token::valDouble(std::string k){
    value.doubleValue = atof(k.c_str());
    kind = TokenKind::val_double;
}

Token *Token::add(Token *tokens, Token *token){
    tokens->next = token;
    token->prev = token;
    token = token->next;
    return token;
}

void Lexer::genTok(){
    if (buf.empty()) {
        return;
    }
    Token *tok = new Token();
    if (state == numberState) {
        tok->valInt(buf);
        tokens = Token::add(tokens, tok);
    }
    if (state == stringState) {
        tok->valStr(buf);
        tokens = Token::add(tokens, tok);
    }
    if (state == doubleState) {
        tok->valStr(buf);
        tokens = Token::add(tokens, tok);
    }
}

void Lexer::clear(){
    state = initState;
    buf = "";
}

void Lexer::input(int c, int next){
    
    location.column++;
    
    if (skipFlag) {
        skipFlag = !skipFlag;
        return;
    }
    
    if (c == '\n') {
        location.line++;
    }
    
    if (commentDepth > 0 && c == '*' && next == '#' && state != stringState) {
        commentDepth--;
        skipFlag = true;
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
    
    if (c == '#' && c == '*') {
        state = longCommentState;
    }
    
    if (isalpha(c)) {
        if (state == initState) {
            state = identifierState;
            buf.push_back(c);
        }
        if (state == identifierState) {
            buf.push_back(c);
        }
        
        return;
    }
    
}
