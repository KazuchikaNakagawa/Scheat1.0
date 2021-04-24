//
//  ScheatBasicStructures.cpp
//  scheat
//
//  Created by かずちか on 2020/09/13.
//

#include "scheat.h"
#include "scheatPriv.hpp"
#include "Lexer.hpp"
#include "ScheatContext.h"
//#include "ScheatNodes.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <map>

using namespace scheat::LegacyScheatParser;
using namespace scheat;
//using namespace scheat::node;

TypeData TypeData::IntType = TypeData("Int", "i32");
TypeData TypeData::StringType = TypeData("String", "%String");
TypeData TypeData::BoolType = TypeData("Bool", "i1");
TypeData TypeData::VoidType = TypeData("Void", "void");

//std::string Function::getMangledName(){
//    std::string base = "@" + return_type.mangledName() + "_";
//    for (int i = 0; i < argTypes.size(); i++) {
//        base = base + argTypes[i].mangledName();
//
//        if (i < argTypes.size()) {
//            base = base + "_";
//        }
//    }
//    return base + mangledName;
//}

Operator *Class::findOperator(string key){
    auto iter = operators.find(key);
    if (iter == operators.end()) {
        return nullptr;
    }
    return iter->second;
}

Class::Class(TypeData *ty) : type(ty){
    properties = {};
    operators = {};
    context = Context::create(ty->name, ScheatContext::global);
};

std::string Function::lltype(){
    std::string base = return_type.ir_used + "(";
    for (int i = 0; i < argTypes.size(); i++) {
        base = base + argTypes[i].ir_used;
        
        if (i < argTypes.size() - 1) {
            base = base + ", ";
        }
    }
    base = base + ")";
    return base;
}

void Class::addProperty(string key, Property p){
    p.index = propCount;
    properties[key] = p;
    bitMap.push_back(p.type);
    propCount++;
    size += p.type.size;
    context->addVariable(key, new Variable(key, p.type));
}

void IRStream::exportTo(std::ofstream &f){
    for (int i = 0; i < irs.size(); i++) {
        auto ir = irs[i];
        if (irs[i].isContext) {
            f << "; context:" << irs[i].context->name << endl;
            irs[i].context->dump(f);
        }else{
            f << irs[i].str;
        }
        
    }
}

void IRStream::printout(){
    for (int i = 0; i < irs.size(); i++) {
        if (irs[i].isContext) {
            irs[i].context->stream_entry.printout();
            irs[i].context->stream_body.printout();
            irs[i].context->stream_tail.printout();
        }else{
            cout << irs[i].str;
        }
    }
}
