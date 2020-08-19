//
//  scheatPriv.hpp
//  scheat
//
//  Created by かずちか on 2020/07/30.
//

/* The classes below are not exported */
#pragma GCC visibility push(hidden)
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


class scheatPriv
{
    public:
    __unavailable
    void HelloWorldPriv(const char *);
};

class StructureData {
    
public:
    virtual std::string mangle();
};

enum class Open {
    _public,
    _private,
    _default,
    _fileprivate,
};

struct MemberData {
    int index;
    StructureData* type;
    Open access;
};

class ClassData : StructureData {
    
public:
    std::map<std::string, MemberData> members;
    std::string mangle();
};


class VariableData {
    
public:
    std::string name;
    StructureData* type;
    Open accessibility;
    llvm::Value *value;
    bool constData;
};

class IRContext {
    static std::map<std::string, StructureData*> types;
    static std::map<std::string, VariableData*> ids;
    static bool isLaunched;
public:
    static void launch();
    static StructureData *getType(std::string, bool);
    static VariableData *getVar(std::string, bool);
    static bool addType(std::string, StructureData *);
    static bool addVar(std::string, VariableData *);
};

class IRBuilderReplica {
    static bool ready;
public:
    static llvm::LLVMContext context;
    static llvm::IRBuilder<> builder;
    static std::unique_ptr<llvm::Module> module;
    static scheat::Scheat *host;
    static void check();
    static void setReady(){
        ready = !ready;
    };
    static bool isReady() { return ready; };
};

#pragma GCC visibility pop
