//
//  CommandLineOptions.hpp
//  productscheat
//
//  Created by かずちか on 2021/01/15.
//

#ifndef CommandLineOptions_hpp
#define CommandLineOptions_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <string>

namespace commandLine {
using namespace std;

enum Type {
    type_int = 0,
    type_string,
    type_bool,
    type_double,
    type_array,
    type_no_args
};

struct Value ;

union Data {
    int ivalue;
    char* svalue;
    bool bvalue;
    double dvalue;
    
    Data(){};
    ~Data() {};
//    Data(const Data&) {};
//    Data(Data &&t) {};
};

struct Value {
    Type type;
    Data data;
    Value(Type t, Data d) : type(t){
        switch (t) {
            case type_int:
                data.ivalue = d.ivalue;
                break;
            case type_string:
                data.svalue = d.svalue;
                break;
            case type_bool:
                data.bvalue = d.bvalue;
                break;
            case type_double:
                data.dvalue = d.dvalue;
                break;
            case type_array:
                
                break;
            case type_no_args:
                
                break;
        }
    }
    Value() : data(Data()) {};
    ~Value() {};
//    Value(const Value &) {};
//    Value(Value &&) {};
};

struct OptionKey {
    string name;
    bool isOptional = false;
    Type type = type_no_args;
    OptionKey(string n, Type t, bool o) : name(n), type(t), isOptional(o){}
    OptionKey(){};
};

extern OptionKey singleArgumentOption(string name, Type type, bool isOptional = false);

struct OptionArgument {
    
};

struct Option : public OptionArgument {
    string name;
    vector<Value> value;
    Option(string n, vector<Value> v) : name(n){
        value = v;
        
    }
    ~Option(){
        
    }
};

struct Argument : public OptionArgument {
    Value value;
};

class Token{
public:
    enum TokenKind {
        tok_int,
        tok_symbol,
        tok_bool,
        tok_double,
        tok_option
    } kind;
    Data data;
    Token() { kind = tok_symbol;
        data.svalue = nullptr; };
    Token(const Token &t){ this->kind = t.kind;
        switch (t.kind) {
            case tok_int:
                this->data.ivalue = t.data.ivalue;
                break;
            case tok_symbol:
                this->data.svalue = (char *)malloc(
                                                   sizeof(char) *
                                                   strlen(t.data.svalue) + 1
                                                   );
                strcpy(data.svalue, t.data.svalue);
                break;
            case tok_bool:
                this->data.bvalue = t.data.bvalue;
            case tok_double:
                this->data.dvalue = t.data.dvalue;
                break;
            case tok_option:
                this->data.svalue = (char *)malloc(
                                                   sizeof(char) *
                                                   strlen(t.data.svalue) + 1
                                                   );
                strcpy(data.svalue, t.data.svalue);
                break;
        }
    };
    
    Token(Token &&t){
        this->kind = t.kind;
        switch (t.kind) {
            case tok_int:
                this->data.ivalue = t.data.ivalue;
                break;
            case tok_symbol:
                this->data.svalue = (char *)malloc(
                                                   sizeof(char) *
                                                   strlen(t.data.svalue) + 1
                                                   );
                strcpy(data.svalue, t.data.svalue);
                break;
            case tok_bool:
                this->data.bvalue = t.data.bvalue;
            case tok_double:
                this->data.dvalue = t.data.dvalue;
                break;
            case tok_option:
                this->data.svalue = (char *)malloc(
                                                   sizeof(char) *
                                                   strlen(t.data.svalue) + 1
                                                   );
                strcpy(data.svalue, t.data.svalue);
                break;
        }
    };
};

class OptionStream {
    
    string buffer;
    vector<OptionKey> opts = {};
    vector<Token> tokens = {};
    void genTok(string&);
    enum kState {
        state_none,
        state_int,
        state_float,
        state_id,
        state_option,
    } state;
public:
    OptionStream(int, const char *[]);
    void addKey(OptionKey);
    void addKeys(vector<OptionKey>);
    void parse();
    vector<Option> getOptions();
    Option *getOption(OptionKey);
    Option *getOption(string, Type);
    vector<Argument> getArguments();
    bool isIncluded(string);
    void printBuffer(){ cout << buffer << endl; };
};

}

#endif /* CommandLineOptions_hpp */
