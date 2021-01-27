//
//  CommandLineOptions.cpp
//  productscheat
//
//  Created by かずちか on 2021/01/15.
//

#include "CommandLineOptions.hpp"

using namespace commandLine;

OptionStream::OptionStream(int argc, const char *argv[]){
    for (int i = 0; i < argc; i++) {
        string k(argv[i]);
        buffer.append(k);
        buffer.append(" ");
    }
}

Option *OptionStream::getOption(string nm, Type type){
    return getOption(OptionKey(nm, type, false));
}

Option *OptionStream::getOption(OptionKey key){
    if (tokens.empty()) {
        return nullptr;
    }
    
    string optname = "";
    int index = 0;
    for (auto token : tokens) {
        index++;
        if (token.kind != Token::tok_option && token.kind != Token::tok_symbol) {
            
            continue;
        }
        //printf("check if '%s' == '%s'\n", key.name.c_str(), token.data.svalue);
        if (string(token.data.svalue) == key.name) {
            optname = token.data.svalue;
            //printf("option was found.\n");
            break;
        }
    }
    
    if (optname.empty()) {
        return nullptr;
    }
    
    if (key.type == type_no_args) {
        return new Option(key.name, {});
    }
    if (true) {
        vector<Value> array;
        for (int i = 0; true; i++) {
            if (index + i > tokens.size() - 1) {
                return nullptr;
            }
            auto token = tokens[index + i];
            auto tp = type_no_args;
            if (token.kind == Token::tok_int) {
                tp = type_int;
            }else if (token.kind == Token::tok_symbol){
                tp = type_string;
            }else if (token.kind == Token::tok_double){
                tp = type_double;
            }else if (token.kind == Token::tok_bool){
                tp = type_bool;
            }else if (token.tok_option) {
                if (i == 0) {
                    return nullptr;
                }
                break;
            }else{
                
            }
            Value v(tp, token.data);
            array.push_back(v);
            if (index + i == tokens.size() - 1){
                break;
            }
        }
        
        return new Option(key.name, array);
    }

    
    
    return new Option(key.name, {});
}

bool OptionStream::isIncluded(string key){
    if (tokens.empty()) {
        return false;
    }
    for (auto token : tokens) {
        if (token.kind != Token::tok_symbol
            &&
            token.kind != Token::tok_option) {
            continue;
        }
        if (strcmp(token.data.svalue, key.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

#define clear { cbuf = ""; state = state_none; }
void OptionStream::genTok(string &cbuf){
    Token empty;
    
    //printf("tok by '%s'\n", cbuf.c_str());
    //printf("state is %d\n", state);
    
    if (cbuf.empty()) {
        clear
        return;
    }
    
    cbuf += "\0";
    
    if (state == state_option) {
        empty.kind = Token::tok_option;
        empty.data.svalue = (char *)malloc(sizeof(char) * cbuf.size() + 1);
        strcpy(empty.data.svalue, cbuf.c_str());
        tokens.push_back(empty);
        clear
        return;
    }else if (state == state_int){
        empty.kind = Token::tok_int;
        empty.data.ivalue = atoi(cbuf.c_str());
        tokens.push_back(empty);
        clear
        return;
    }else if (cbuf == "YES"){
        empty.kind = Token::tok_bool;
        empty.data.bvalue = true;
        clear
        return;
    }else if (cbuf == "NO"){
        empty.kind = Token::tok_bool;
        empty.data.bvalue = false;
        tokens.push_back(empty);
        clear
        return;
    }else{
        empty.kind = Token::tok_symbol;
        empty.data.svalue = (char *)malloc(sizeof(char) * cbuf.size() + 1);
        strcpy(empty.data.svalue, cbuf.c_str());
        tokens.push_back(empty);
        clear
        return;
    }
}

void OptionStream::parse(){
    
    string cbuf = "";
    for (char c : buffer) {
        if (c == ' ') {
            genTok(cbuf);
            continue;
        }
        if (isalpha(c)) {
            cbuf.push_back(c);
            if (state == state_none) {
                state = state_id;
                continue;
            }else if (state == state_int){
                state = state_id;
                continue;
            }
            continue;
        }
        
        if (isnumber(c)) {
            cbuf.push_back(c);
            if (state == state_none) {
                state = state_int;
                
            }
            continue;
        }
        
        if (c == '-') {
            if (state == state_none) {
                genTok(cbuf);
            }
            cbuf.push_back(c);
            if (state == state_none) {
                state = state_option;
                //printf("- detected\n");
            }
            continue;
        }
        
        cbuf.push_back(c);
    }
    genTok(cbuf);
    if (tokens.empty()) {
        return;
    }
#undef clear
}

OptionKey singleArgumentOption(string name, Type type, bool isOptional = false){
    OptionKey key;
    key.type = type;
    key.isOptional = isOptional;
    key.name = name;
    return key;
}
