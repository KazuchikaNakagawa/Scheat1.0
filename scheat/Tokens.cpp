//
//  Tokens.cpp
//  scheat
//
//  Created by かずちか on 2020/09/02.
//

#include <stdio.h>
#include "Lexer.hpp"
#include "ScheatStatics.h"

void scheat::Tokens::skip(){
    seek_ptr = seek_ptr->next;
}

void scheat::Tokens::back(){
    seek_ptr = seek_ptr->prev;
}

void scheat::eatThis(scheat::Token *&tokref){
    tokref = tokref->next;
    if (tokref == nullptr) {
        statics::scheato->Log(statics::scheato->location, __FILE_NAME__, __LINE__, "Lex ended");
        
    }
}
