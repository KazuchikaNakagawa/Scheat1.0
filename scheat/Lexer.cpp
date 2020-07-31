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
    
    
    state = initState;
    this->host = host;
}

void Lexer::lex(std::ifstream stream){
    if (!stream.is_open()) {
        host->FatalError("input file is not open --Lexer", __LINE__);
    }
    int c;
    while (c = stream.get(), c != EOF) {
        input(c);
    }
}

void Lexer::input(int c){
    if (state == longCommentState) {
        return;
    }
}
