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

#define unique(a) std::unique_ptr<a>

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
    __deprecated IntToken(Token *token) { i = token->value.intValue; };
    llvm::Value * codegen() override;
    ~IntToken() {};
    void dump() override{
        printf(" Int(%d) ", i);
    }
    static unique(IntToken) init(Token *k) {
        return std::make_unique<IntToken>(k);
    };
};

class IdentifierToken : public Expression {
    std::string idData;
public:
    __deprecated IdentifierToken(Token *id);
    void dump() override{
        printf(" ID(%s) ", idData.c_str());
    }
    llvm::Value * codegen() override;
    static unique(IdentifierToken) init(Token * id){
        return std::make_unique<IdentifierToken>(id);
    };
    std::string getIdData() const{ return idData; };
};

class PrimaryExprIdentifier : public Expression {
    unique(PrimaryExprIdentifier) body;
    unique(IdentifierToken) child;
public:
    __deprecated PrimaryExprIdentifier(unique(IdentifierToken) add, unique(PrimaryExprIdentifier) body = nullptr);
    llvm::Value * codegen() override;
};

IdentifierToken::IdentifierToken(Token *id){
    idData = id->value.strValue;
}

llvm::Value *IntToken::codegen(){
    return llvm::ConstantInt::get(IRBuilderReplica::context, llvm::APInt(32, i));
}

llvm::Value * IdentifierToken::codegen(){
    IRBuilderReplica::host->FatalError(__LINE__, "this codegen() is not to be called.");
    return NULL;
}

PrimaryExprIdentifier::PrimaryExprIdentifier(unique(IdentifierToken) add, unique(PrimaryExprIdentifier) body){
    if (body != nullptr) this->body = std::move(body);
    child = std::move(add);
}

llvm::Value *PrimaryExprIdentifier::codegen(){
    if (body == nullptr) {
        return IRContext::getVar(child->getIdData(), false)->value;
    }else{
        
    }
    return nullptr;
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


