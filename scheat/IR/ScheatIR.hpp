//
//  ScheatIR.hpp
//  scheat
//
//  Created by かずちか on 2021/05/29.
//

#ifndef ScheatIR_hpp
#define ScheatIR_hpp

#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "ScheatTypes.h"

using namespace std;
namespace scheat {

/*
 Scheatの中間表現
 型の解決まで行い、最終的にLLVMIRへ持ち込む
 ジェネリクスや自動キャスト、Magic型に必要
 Scheat→ScheatIR→LLVM IR
 */

namespace IR {

struct IRValue;

struct Ofstream;

enum class ExportType : int {
    LLVM_Text,
    Asm __unavailable,
    LLVMIR __unavailable,
};

struct IR;

// Scope
// label + inst
class Scope;

class ContextKeys {
public:
    static const std::string Register;
    static const std::string If;
};

class GlobalContext;

// Context
// function, global
class Context{
    map<string, Scope*> scopeDict;
    vector<Scope *> scopes;
    Context *base;
    map<string , IR> variableRegisters;
    // saves index of if, for...
    map<string, uint32_t> keys;
public:
    void createScope(string);
    Scope *getScope(string);
    IR registerInst(Scope *target);
    IR namedRegisterInst(Scope *target);
    IR allocateInst(Scope* target, TypeData type, string name);
    IR storeInst(Scope* target, IR ptr, IR val);
    IR varInst(Scope* target, string name, TypeData type);
    IR loadInst(Scope* target, IR value);
    IR accessInst(GlobalContext *,Scope *target, IR value, string name);
    IR branchInst(IR condition, Scope *body, Scope *el);
};



class LocalContext;

class FunctionContext;

}

}
#endif /* ScheatIR_hpp */
