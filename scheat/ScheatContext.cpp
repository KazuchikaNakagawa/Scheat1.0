//
//  ScheatContext.cpp
//  scheat
//
//  Created by かずちか on 2020/09/13.
//

#include <stdio.h>
#include "ScheatContext.h"
#include "ScheatBasicStructures.h"
#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace scheat;
using namespace scheat::basics;

Class *Context::findClass(std::string key){
    return classes[key];
}

Function *Context::findFunc(std::string key){
    return funcs[key];
}

void Context::dump(std::ofstream &f){
    
    // typename std::map<std::string, Class *>::iterator
    auto iter = begin(classes);
    while (iter != classes.end()) {
        auto pair = *iter;
        pair.second->context->dump(f);
        iter = std::next(iter);
    }
    f << "; " << name << "\n";
    stream_entry.exportTo(f);
    stream_body.exportTo(f);
    stream_tail.exportTo(f);
    
    auto iter_f = begin(funcs);
    while (iter_f != funcs.end()) {
        auto pair = *iter_f;
        pair.second->context->dump(f);
    }
    
    for (auto p = funcs.begin(); p != funcs.end(); p = std::next(p)) {
        (*p).second->context->dump(f);
    }
    
}
