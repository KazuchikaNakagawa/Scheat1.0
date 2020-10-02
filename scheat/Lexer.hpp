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
#include "ScheatBasicStructures.h"
#include "Classes.h"
#include <fstream>

namespace scheat {
class Scheat;

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

class Lexer : public ScheatLexer {
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
    bool AccessTokFlag = false;
public:
    
    // initializer
    // Scheat object for version management
    Lexer(scheat::Scheat*);
    
    // lex(ifstream)
    // lex file and return token
    void lex(std::ifstream &);
    
    void lex(std::string);
    
    static Token *lexString(Scheat *, std::string);
    
    static Token *lexThis(Scheat *);
    void lex(Scheat *sch) override {
        lexThis(sch);
    };
    Token * getNextTok() override{
        tokens = tokens->next;
        return tokens;
    }
    Token * eatThisTok() override{
        tokens = tokens->next;
        return tokens;
    }
    // this function has fatal probrem
    // void addToken();
    void clear();
    
    Token *getTokens() { return tokens; };
};

}
}

#endif /* Lexer_hpp */
