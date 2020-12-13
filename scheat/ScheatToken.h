//
//  ScheatToken.h
//  ProjectScheat
//
//  Created by かずちか on 2020/12/13.
//

#ifndef ScheatToken_h
#define ScheatToken_h
#include <string>

namespace scheat {

struct SourceLocation {
    int line;
    int column;
    SourceLocation(){
        line = 1;
        column = 0;
    }
    ~SourceLocation() = default;
};

enum class TokenKind : int {
    
    val_identifier,
    val_num,
    val_str,
    val_double,
    val_bool,
    val_operator,
    
    tok_this,
    tok_the,
    tok_is,
    tok_of,
    tok_period,
    tok_comma,
    tok_range,
    tok_if,
    tok_do,
    tok_paren_l,
    tok_paren_r,
    tok_brace_l,
    tok_brace_r,
    tok_access,
    tok_external,
    tok_import,
    tok_export,
    tok_to,
    tok_with,
    
    embbed_func_print,
    embbed_func_import,
    embbed_func_free,
    embbed_func_assemble,
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
    std::string encodeOperator();
    void out();
    Token(){
        next = nullptr;
        prev = nullptr;
        kind = TokenKind::val_num;
        value.intValue = 0;
        location = SourceLocation();
    }
    void release(){
        
        if (prev != nullptr) {
            prev->release();
            prev = nullptr;
        }
        delete this;
    }
    Token(Token &&) {};
    Token(const Token &) {};
};

extern void eatThis(Token *&);

class Tokens {
    Token *tokens;
    Token *seek_ptr;
public:
    Token *ptr() { return seek_ptr; };
    void skip();
    void back();
    Tokens *operator += (Token *rhs){
        rhs->prev = this->tokens->last();
        this->tokens->last()->next = rhs;
        tokens = tokens->next;
        return this;
    };
    Token *operator[](unsigned int index){
        Token *t = tokens->first();
        for (int i = 0; i != index; i++) {
            t = t->next;
        }
        return t;
    };
};

}

#endif /* ScheatToken_h */
