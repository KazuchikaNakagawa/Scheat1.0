//
//  ScheatContext.cpp
//  scheat
//
//  Created by かずちか on 2020/09/13.
//

#include <stdio.h>
#include "ScheatContext.h"
#include "ScheatStatics.h"
//#include "ScheatNodes.h"
#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace scheat;
//using namespace scheat::node;

Class *Context::findClass(std::string key){
    auto index = classes.find(key);
    if (index == classes.end()) {
        return base->findClass(key);
    }
    return index->second;
}

string Context::getRegister(){
    std::string v = "%r" + std::to_string(rnum);
    rnum++;
    return v;
}

Context *Context::create(std::string name, Context *parents){
    Context *cnt = new Context(name, parents);
    if (!parents) {
        ScheatContext::contextCenter.push_back(cnt);
    }
    return cnt;
}

Variable *Context::findVariable(std::string key){
    Variable *v = variables[key];
    if (v != nullptr) {
        return v;
    }else{
        if (base == nullptr) {
            return nullptr;
        }
        v = base->findVariable(key);
        return v;
    }
}

void ScheatContext::exportTo(ofstream &f){
    for (auto con : contextCenter) {
        if (con->name != "global") f << "; " << con->name << endl;
        con->dump(f);
        f << endl;
    }
}

Function::Function(TypeData type, std::string nm, bool demangle) : return_type(type){
    mangledName = ScheatContext::local()->name + "_" + nm;
    if (!demangle) {
        mangledName = nm;
    }
    name = nm;
    argTypes = {};
    context = Context::create(nm, ScheatContext::local());
}

void Context::addFunction(std::string key, Function *value){
    funcs[key] = value;
}

Function *Context::findFunc(std::string key, vector<TypeData> ts){
    auto ptr = funcs.find(key);
    if (ptr == this->funcs.end()) {
        if (base == nullptr) {
            return nullptr;
        }
        return base->findFunc(key, ts);
    }
    if (!(*ptr).second->ifTypesAdjust(ts)) {
        return nullptr;
    }
    return funcs[key];
}

void Context::addClass(std::string key, Class *value){
    classes[key] = value;
    classes[ScheatContext::getNamespace() + "_" + key] = value;
}

Function *Context::createFunction(TypeData *return_type,
                                  string name,
                                  string ir_name,
                                  vector<TypeData> arguments){
    auto f = new Function(*return_type, ir_name, false);
    f->argTypes = arguments;
    return f;
    
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
    
    f << "; " << name << "\n";
    stream_entry.exportTo(f);
    stream_body.exportTo(f);
    stream_tail.exportTo(f);
    
    for (auto pair : funcs) {
        pair.second->context->dump(f);
    }
    
}

void LocalContext::_break(){
    for (auto pair : newVs) {
        if (pair->type.ir_used.find("*") != string::npos) {
            auto r = getRegister();
            stream_body << r << " = bitcast " << pair->type.ir_used << "* " << pair->mangledName << " to i8*\n";
            stream_body << "call void @ScheatPointer_unref(i8* " << r << ")\n";
        }else{
            auto r = getRegister();
            stream_body << r << " = bitcast " << pair->type.ir_used << "* " << pair->mangledName << " to i8*\n";
            stream_body << "call void @" << pair->type.name << "_deinit(i8* " << r << ")\n";
        }
    }
}

void LocalContext::addFunction(std::string, Function *){
    scheato->FatalError(SourceLocation::OUTOFFILELOCATION, __FILE_NAME__, __LINE__, "compiler seems not to be working well.");
}

void LocalContext::addClass(std::string, Class *){
    scheato->FatalError(SourceLocation::OUTOFFILELOCATION, __FILE_NAME__, __LINE__, "compiler seems not to be working well.");
}


