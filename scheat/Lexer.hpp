//
//  Lexer.hpp
//  scheat
//
//  Created by かずちか on 2020/07/31.
//

#ifndef Lexer_hpp
#define Lexer_hpp

#include <stdio.h>
#include <fstream>


struct SourceLocation {
    int line;
    int column;
    SourceLocation(){
        line = 1;
        column = 1;
    }
    ~SourceLocation() = default;
};


namespace scheat {
class Scheat;


enum class TokenKind : int {
    
    val_identifier,
    val_num,
    val_str,
    val_double,
    val_bool,
    val_operator,
    
    tok_this,
    tok_that,
    tok_is,
    tok_period,
    tok_comma,
    tok_external,
    
    embbed_func_print,
    embbed_func_import,
    embbed_func_free,
};

union TokenValue {
    std::string strValue;
    int intValue;
    bool boolValue;
    double doubleValue;
    TokenValue(){
        doubleValue = 0.0;
    }
    ~TokenValue(){
        
    }
    TokenValue(TokenValue &&){
        
    }
    TokenValue(const TokenValue &){
        
    }
};

/// Token --Scheat's token.
struct Token {
    /// Token has chain structure
    
    // next : next Token pointer
    Token *next;
    
    // prev : previous Token pointer
    Token *prev;
    
    // TokenKind : presents type of Token.
    TokenKind kind;
    
    // TokenValue: value has 4 types.
    // scheat.int (llvm i32)
    // scheat.str (llvm i8*) or (llvm %scheatstd.string)
    // scheat.bool (llvm i1)
    // scheat.double (llvm double)
    TokenValue value;
    
    // last : returns last token
    Token *last();
    
    // first : returns first token
    Token *first();
    
    // location: SourceLocation
    // column , line
    SourceLocation location;
    void valInt(std::string);
    void valStr(std::string);
    void valBool(std::string);
    void valDouble(std::string);
    static Token *add(Token *, Token *);
    void enumerate();
    void out();
    Token(){
        next = nullptr;
        prev = nullptr;
        kind = TokenKind::val_num;
        value.intValue = 0;
        location = SourceLocation();
    }
    Token(Token &&) {};
    Token(const Token &) {};
};

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
    SourceLocation location;
    Token *tokens;
    void input(int c, int next);
    void genTok();
    int commentDepth;
    bool skipFlag;
public:
    
    // initializer
    // Scheat object for version management
    Lexer(scheat::Scheat*);
    
    // lex(ifstream)
    // lex file and return token
    void lex(std::ifstream);
    
    void lex(std::string);
    
    // this function has fatal probrem
    // void addToken();
    void clear();
    
    Token *getTokens() { return tokens; };
};


}

#endif /* Lexer_hpp */
