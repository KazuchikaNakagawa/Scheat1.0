//
//  Lexer.hpp
//  scheat
//
//  Created by かずちか on 2020/07/31.
//

#ifndef Lexer_hpp
#define Lexer_hpp

#include <stdio.h>
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
    tok_external,
};

union TokenValue {
    std::string strValue;
    int intValue;
    bool boolValue;
    double doubleValue;
};

struct Token {
    Token *next;
    Token *prev;
    TokenKind kind;
    TokenValue value;
    Token *last();
    Token *first();
    SourceLocation location;
};

enum LexerState {
    commentState,
    longCommentState,
    numberState,
    doubleState,
    stringState,
    identifierState,
    initState,
    ErrorState,
};

class Lexer {
    std::string buf;
    LexerState state;
    scheat::Scheat *host;
    SourceLocation location;
    Token *tokens;
    void input(int c);
    void genTok();
public:
    Lexer(scheat::Scheat*);
    void lex(std::ifstream);
    void addToken();
    void clear();
    Token *getTokens() { return tokens; };
};


#endif /* Lexer_hpp */
