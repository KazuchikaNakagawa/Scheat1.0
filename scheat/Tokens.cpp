//
//  Tokens.cpp
//  scheat
//
//  Created by かずちか on 2020/09/02.
//

#include <stdio.h>
#include "Lexer.hpp"


void scheat::Tokens::skip(){
    seek_ptr = seek_ptr->next;
}

void scheat::Tokens::back(){
    seek_ptr = seek_ptr->prev;
}

void eatThis(scheat::Token *&tokref){
    tokref = tokref->next;
}
