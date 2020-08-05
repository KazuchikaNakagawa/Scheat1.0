//
//  Nodes.cpp
//  scheat
//
//  Created by かずちか on 2020/07/30.
//

#include <stdio.h>
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "scheat.hpp"
#include "scheatPriv.hpp"

using namespace scheat;

class Statement {
    
public:
    virtual ~Statement() = default;
    virtual void dump() = 0;
    virtual llvm::Value *codegen() = 0;
};

class Expression {
public:
    virtual ~Expression() = default;
    virtual void dump() = 0;
    virtual llvm::Value *codegen() = 0;
};

class IntToken : public Expression {
    int i;
public:
    IntToken(Token *token) { i = token->value.intValue; };
    llvm::Value * codegen() override;
};

class VariableToken : public Expression {
    VariableData* idData;
public:
    VariableToken(Token *id);
    llvm::Value * codegen() override;
};

VariableToken::VariableToken(Token *id){
    idData = IRContext::getVar(id->value.strValue, false);
    if (idData->accessibility == Open::_private) {
        
    }
    if (idData == NULL){
        IRBuilderReplica::host->FatalError(__LINE__, "in %d.%d %s is undefined", id->location.line, id->location.column, id->value.strValue.c_str());
    }

}

llvm::Value *IntToken::codegen(){
    return llvm::ConstantInt::get(IRBuilderReplica::context, llvm::APInt(32, i));
}

llvm::Value * VariableToken::codegen(){
    return IRBuilderReplica::builder.CreateLoad(idData->value, idData->name);
}

// -------------------------------------------------------------

std::map<std::string, StructureData*> IRContext::types;
std::map<std::string, VariableData*> IRContext::ids;

StructureData *IRContext::getType(std::string key, bool force = false){
    if (types.find(key) == types.end()) {
        if (force) {
            printf("Error: forced type not found.\n");
            exit(0);
        }else{
            return nullptr;
        }
    }
    
    return types[key];
}

VariableData *IRContext::getVar(std::string key, bool force = false){
    
    if (ids.find(key) == ids.end()) {
        if (force) {
            printf("Error: forced variable not found.\n");
            exit(0);
        }else{
            return nullptr;
        }
    }
    
    return ids[key];
}

bool IRContext::isLaunched = false;

bool IRContext::addType(std::string name, StructureData *value){
    if (!isLaunched) {
        printf("ERROR: TO DEVELOPER\n");
        printf("    YOU NEED TO CALL IRContext::launch() FIRST.\n");
        exit(0);
    }
    if (types.find(name) != types.end()) {
        return false;
    }
    types.insert(std::make_pair(name, value));
    return true;
}

bool IRContext::addVar(std::string name, VariableData *value){
    if (!isLaunched) {
        printf("ERROR: TO DEVELOPER\n");
        printf("    YOU NEED TO CALL IRContext::launch() FIRST.\n");
        exit(0);
    }
    if (ids.find(name) != ids.end()) {
        return false;
    }
    ids.insert(std::make_pair(name, value));
    return true;
}

void IRContext::launch(){
    types = {};
    ids = {};
    isLaunched = true;
}


