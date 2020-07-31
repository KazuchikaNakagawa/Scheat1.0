//
//  Lexer.cpp
//  scheat
//
//  Created by かずちか on 2020/07/31.
//

#include "Lexer.hpp"
#include "scheat.hpp"
#include <fstream>

enum TokenKind {
    val_identifier,
    val_num,
    val_str,
    val_double,
    
    tok_this,
    tok_that,
    tok_is,
};

struct Token {
    Token *next;
    Token *prev;
    
};

class Lexer {
    std::string buf;
public:
    void lex(std::ifstream);
    
};
