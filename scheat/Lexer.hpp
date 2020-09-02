//
//  Lexer.hpp
//  scheat
//
//  Created by かずちか on 2020/07/31.
//

#ifndef Lexer_hpp
#define Lexer_hpp

#include <stdio.h>
#include "ScheatObjects.h"
#include "BasicStructures.h"
#include <fstream>

namespace scheat {
class Scheat;


enum LexerState {
    commentState,
    longCommentState,
    numberState,
    doubleState,
    stringState,
    identifierState,
    operatorState,
    initState,
    ErrorState,
};

class Lexer {
    std::string buf;
    LexerState state;
    scheat::Scheat *host;
    SourceLocation &location;
    Token *tokens;
    void input(int c, int next);
    void genTok();
    int commentDepth;
    bool skipFlag;
    bool isPossibleForPPPTok = false;
public:
    
    // initializer
    // Scheat object for version management
    Lexer(scheat::Scheat*);
    
    // lex(ifstream)
    // lex file and return token
    void lex(std::ifstream &);
    
    void lex(std::string);
    
    // this function has fatal probrem
    // void addToken();
    void clear();
    
    Token *getTokens() { return tokens; };
};


}

#endif /* Lexer_hpp */
