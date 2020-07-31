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
