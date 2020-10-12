//
//  BasicStructures.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/26.
//

#ifndef BasicStructures_h
#define BasicStructures_h

#include <map>
#include <vector>
#include "ScheatObjects.h"
namespace scheat {

class Context;


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

namespace basics{

class IRStream {
public:
    std::vector<std::string> irs;
    IRStream& operator <<(std::string v){
        irs.push_back(v);
        return *this;
    };
    IRStream& operator <<(const char v[]){
        std::string vs(v);
        irs.push_back(vs);
        return *this;
    }
    void exportTo(std::ofstream &f);
    IRStream(){
        irs = {};
    };
};

class TypeData {
    
public:
    std::string ir_used;
    std::string name;
    
    bool operator==(TypeData *rhs){
        return this->name == rhs->name
        && this->mangledName() == rhs->mangledName();
    }
    bool operator==(std::string s){
        return this->name == s || this->mangledName() == s;
    }
    bool operator!=(TypeData *rhs){
        return this->name != rhs->name
        || this->mangledName() != rhs->mangledName();
    }
    std::string mangledName() const{ return ir_used; };
    TypeData(std::string nm){
        name = nm;
        
        char *n_b = (char *)malloc(sizeof(nm) + 1);
        int i = 0;
        strcpy(n_b, nm.c_str());
        if (nm == "double") {
            ir_used = "double";
            return;
        }
        else if (sscanf(n_b, "i%d", &i) == 1) {
            ir_used = nm;
            free(n_b);
            return;
        }else{
            free(n_b);
        }
        ir_used = "%" + nm;
        
    }
    
    TypeData() {};
    
    TypeData(std::string a, std::string ir) : name(a), ir_used(ir) {};
};

class Operator {
    
public:
    enum{
        prefix,
        infix,
        postfix
    } position;
    
    enum{
        primary,
        secondary,
        term
    } precidence;
    
    std::string value;
    
    TypeData return_type;
    // if postfix operator, this will be nullptr
    TypeData *rhs_type;
    // if prefix operator, this will be nullptr
    TypeData *lhs_type;
    std::string func_name;
    Operator(std::string func, std::string symbols) : func_name(func), value(symbols){
        
    }
    Operator(){
        
    }
};

class Function{
    std::string mangledName;
public:
    
    std::string name;
    std::string getMangledName();
    TypeData return_type;
    std::vector<TypeData> argTypes;
    std::string lltype();
    std::string codegen(IRStream &);
    Context *context;
    Function(std::string ,std::string);
};

class Variable {
    
public:
    std::string mangledName;
    // do NOT think about pointer. it shows what value it has.
    TypeData type;
    Variable(std::string s, TypeData t) : mangledName(s), type(t) {
        
    }
};

class Property {
    
public:
    unsigned int index;
    TypeData type;
    Property(TypeData ty) : type(ty) { index = 0; };
};

class Class {
    
    unsigned int propCount = 0;
public:
    std::map<std::string, unsigned int> properties;
    std::map<std::string, Operator*> operators;
    std::vector<TypeData *> bitMap;
    Class *parentClass;
    TypeData *type;
    Context *context;
    Operator *findOperator(string);
    Class(TypeData *ty);
};
}

}

#endif /* BasicStructures_h */
