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

void Lexer::genTok(){
    
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
    
    if (commentDepth > 0 && c == '*' && next == '#') {
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
    }
    
    if (c == '"' && state == stringState) {
        buf.push_back(c);
        genTok();
    }
    
    if (c == '"') {
        genTok();
        buf.push_back(c);
        state = stringState;
    }
    
    if (state == stringState) {
        buf.push_back(c);
    }
    
    if (c == '\n') {
        genTok();
    }
    
    if (c == ' ') {
        genTok();
    }
    
    if (c == '\t') {
        genTok();
    }
    
    if (c == '#' && c == '*') {
        state = longCommentState;
    }
    
}
