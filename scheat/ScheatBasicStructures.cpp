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
#include "ScheatNodes.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <map>

using namespace scheat::LegacyScheatParser;
using namespace scheat;
using namespace scheat::basics;
using namespace scheat::node;

std::string Function::getMangledName(){
    std::string base = return_type.mangledName() + "_";
    for (int i = 0; i < argTypes.size(); i++) {
        base = base + argTypes[i].mangledName();
        
        if (i < argTypes.size()) {
            base = base + "_";
        }
    }
    return mangledName + base;
}

std::string basics::Function::lltype(){
    std::string base = return_type.mangledName() + "(";
    for (int i = 0; i < argTypes.size(); i++) {
        base = base + argTypes[i].mangledName();
        
        if (i < argTypes.size()) {
            base = base + ", ";
        }
    }
    base = base + ")";
    return base;
}

void IRStream::exportTo(std::ofstream &f){
    for (int i = 0; i < irs.size(); i++) {
        f << irs[i];
    }
}
