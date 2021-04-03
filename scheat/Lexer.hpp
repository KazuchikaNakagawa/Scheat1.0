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
#include "ScheatToken.h"
#include "Classes.h"
#include <fstream>
#include "ScheatStd.hpp"

namespace scheat {
class _Scheat;
using namespace scheatSTD;

namespace lexer {
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
    scheat::_Scheat *host = nullptr;
    SourceLocation &location;
    Token *tokens = nullptr;
    void input(int c, int next);
    void genTok();
    void addEOFToken();
    int commentDepth;
    bool skipFlag;
    bool isPossibleForPPPTok = false;
    bool AccessTokFlag = false;
public:
    
    // initializer
    // Scheat object for version management
    Lexer(scheat::_Scheat*);
    
    // lex(ifstream)
    // lex file and return token
    void lex(std::ifstream &);
    
    void lex(std::string);
    
    static Token *lexString(_Scheat *, std::string);
    
    static Token *lexThis(_Scheat *);
    void lex(_Scheat *sch) {
        sch->tokens = lexThis(sch);
    };
    Token * getNextTok();
    Token * eatThisTok();
    // this function has fatal probrem
    // void addToken();
    void clear();
    
    void clearTokens() ;
    
    Token *getTokens();
};

}
}

#endif /* Lexer_hpp */
