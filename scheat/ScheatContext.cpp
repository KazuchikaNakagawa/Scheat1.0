//
//  ScheatContext.cpp
//  scheat
//
//  Created by かずちか on 2020/09/13.
//

#include <stdio.h>
#include "ScheatContext.h"
#include "ScheatBasicStructures.h"
#include "ScheatStatics.h"
#include "ScheatNodes.h"
#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace scheat;
using namespace scheat::basics;
using namespace scheat::node;
using namespace statics;
using statics::scheato;

Class *Context::findClass(std::string key){
    return classes[key];
}

static NodeData* castType(IRStream &f, NodeData *data, TypeData *to){
    if (data->size.ir_used[0] == '%') {
        auto a = ScheatContext::global->findClass(data->size.name);
        if (a == nullptr) {
            scheato->FatalError(scheato->location, __FILE_NAME__, __LINE__, "Domestic Error. %s is undefined.", data->size.ir_used.c_str());
        }
        if (a->type->name == to->name) {
            auto r = ScheatContext::local()->getRegister();
            f << r << " = bitcast " << data->size.ir_used << "* " << data->value << " to " << to->ir_used << "*\n";
        }
    }
    if (data->size.ir_used[0] == 'i') {
        
    }
    return nullptr;
};

Function::Function(std::string type, std::string nm) : return_type(type){
    mangledName = "@" + ScheatContext::local()->name + "_" + nm;
    name = nm;
    argTypes = {};
    context = Context::create(nm, ScheatContext::local());
}

void Context::addFunction(std::string key, Function *value){
    funcs[key] = value;
}

Function *Context::findFunc(std::string key){
    return funcs[key];
}

void Context::addClass(std::string key, Class *value){
    classes[key] = value;
}


void Context::addVariable(std::string key, Variable *value){
    variables[key] = value;
}

bool Context::isExists(std::string key){
    if (variables[key] != nullptr) {
        return true;
    }
    if (funcs[key] != nullptr) {
        return true;
    }
    return false;
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
