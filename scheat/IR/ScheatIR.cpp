//
//  ScheatIR.cpp
//  scheat
//
//  Created by かずちか on 2021/05/29.
//

#include "ScheatIR.hpp"
#include <fstream>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Linker/Linker.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/LinkAllIR.h>
#include <system_error>
#include <sstream>
#include <unistd.h>


int k();
using namespace std;
using namespace scheat::IR;

const string ContextKeys::Register = "register";
const string ContextKeys::If = "if";

struct scheat::IR::Ofstream {
    ofstream &ofs;
    unsigned int indent;
    bool linebeginning = false;
    void startLine(){
        char *p = (char*)malloc(indent*4+1);
        memset(p, 0, indent*4+1);
        memchr(p, ' ', indent*4);
        ofs << p;
        free(p);
        linebeginning = true;
    };
    Ofstream& operator <<(string d){
        if (!linebeginning) {
            startLine();
        }
        ofs << d;
        return *this;
    }
    Ofstream& operator <<(const char s[]){
        if (!linebeginning) {
            startLine();
        }
        ofs << s;
        return *this;
    }
    Ofstream& operator <<(int i){
        if (!linebeginning) {
            startLine();
        }
        ofs << i;
        return *this;
    }
    void endLine(){
        linebeginning = false;
    }
};

struct scheat::IR::IRValue {
    string v;
    scheat::TypeData type;
    IRValue(){
        type = TypeData::VoidType;
    }
    IRValue(string s, scheat::TypeData t){
        v = s;
        type = t;
    }
    string value(){
        return type.ir_used + " " + v;
    }
};

class scheat::IR::IR {
    
public:
    virtual IRValue writeAssembly(Ofstream &){
        perror("Writing assembly is not available yet.");
        return IRValue();
    };
    virtual llvm::Value* writeLLVMIR(llvm::LLVMContext&,
                                     llvm::IRBuilder<>&,
                                     unique_ptr<llvm::Module>&){
        perror("Writing LLVM IR is not available yet.");
        return nullptr;
    };
    virtual IRValue writeTextLLVMIR(Ofstream &){
        perror("This instruction is not for this function");
        return IRValue();
    };
    virtual ~IR() {};
};

class RegisterInst : public IR::IR {
    int num;
public:
    RegisterInst(int index){
        num = index;
    }
    
    IRValue writeTextLLVMIR(Ofstream &) override{
        return IRValue("%" + to_string(num), scheat::TypeData::VoidType);
    }
};

class NamedRegisterInst : public IR::IR {
    string name;
public:
    NamedRegisterInst(string name){
        this->name = name;
    }
    
    IRValue writeTextLLVMIR(Ofstream &) override{
        return IRValue("%" + name, scheat::TypeData::VoidType);
    }
};

class VariableRegisterInst : public RegisterInst {
    string name;
    scheat::TypeData type;
public:
    VariableRegisterInst(string name, scheat::TypeData type) : RegisterInst(0){
        this->name = name;
        this->type = type;
    }
    
    IRValue writeTextLLVMIR(Ofstream &f) override{
        return IRValue("%" + name, scheat::TypeData::VoidType);
    }
};

class scheat::IR::Scope : public IR {
    vector<IR> irs;
    string label;
public:
    Scope(string s){
        label = s;
    }
    void exportTo(ofstream &);
    friend class Context;
};

class scheat::IR::GlobalContext : Context {
    map<string, map<string, pair<int, TypeData>>> properties;
    
public:
    void addProperty(TypeData className, string name, int index, TypeData type);
    pair<int, TypeData> getProperty(TypeData classData, string name){
        if (properties[classData.ir_used].find(name) == properties[classData.ir_used].end()) {
            perror("ScheatIR:Out of range Error");
        }
        return properties[classData.ir_used][name];
    }
};

void GlobalContext::addProperty(TypeData className, string name, int index, TypeData type){
    properties[className.ir_used][name] = make_pair(index, type);
}

class AllocateInst: public IR {
    scheat::TypeData type;
    RegisterInst registerInst;
public:
    AllocateInst(scheat::TypeData size, RegisterInst reg) : registerInst(reg){
        type = size;
    };
    IRValue writeTextLLVMIR(Ofstream &f) override{
        auto r = registerInst.writeTextLLVMIR(f);
        f << r.v << " = alloca " << type.ir_used;
        f.endLine();
        r.type = type;
        return r;
    }
    llvm::Value * writeLLVMIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, unique_ptr<llvm::Module>& module) override{
        
        return builder.CreateAlloca(llvm::Type::getInt32Ty(context), registerInst.writeLLVMIR(context, builder, module));
    }
    
};

class StoreInst : public IR {
    IR value;
    IR pointer;
public:
    StoreInst(IR v, IR p) : value(v), pointer(p){
        
    }
    
    IRValue writeTextLLVMIR(Ofstream &f) override{
        f << "store " << value.writeTextLLVMIR(f).value() << ", " << pointer.writeTextLLVMIR(f).value();
        f.endLine();
        return IRValue();
    }
};

class AccessInst : public IR {
    IR resultRegister;
    IR value;
    string name;
    GlobalContext *global;
public:
    AccessInst(GlobalContext *m,IR ptr, string id, IR registerInst){
        value = ptr;
        name = id;
        global = m;
        resultRegister = registerInst;
    }
    IRValue writeTextLLVMIR(Ofstream &f) override{
        auto tvalue = value.writeTextLLVMIR(f);
        auto pair = global->getProperty(tvalue.type.loaded(), name);
        auto rvalue = resultRegister.writeTextLLVMIR(f);
        f << rvalue.v << " = getelementptr " << tvalue.type.loaded().ir_used << ", "
        << tvalue.value() << ", i32 0, i32 " << pair.first;
        f.endLine();
        rvalue.type = pair.second.pointer();
        return rvalue;
    }
};

class LoadInst : public IR {
    IR ptr;
    IR reg;
public:
    LoadInst(IR reg, IR ptr){
        this->ptr = ptr;
        this->reg = reg;
    }
    IRValue writeTextLLVMIR(Ofstream &f) override{
        auto v = ptr.writeTextLLVMIR(f);
        f << "load " << v.type.loaded().ir_used << ", " << v.value();
        f.endLine();
        return IRValue(reg.writeTextLLVMIR(f).v, v.type.loaded());
    }
    
};

class BranchInst : public IR {
    IR cond;
public:
    
};

scheat::IR::Scope *scheat::IR::Context::getScope(string key){
    //
    auto p = scopeDict.find(key);
    if (p == scopeDict.end()) {
        auto scp = new Scope("");
        scopeDict[key] = scp;
        return scp;
    }
    return p->second;
}

IR Context::registerInst(Scope *target){
    int k = keys["register"];
    keys["register"]++;
    return RegisterInst(k);
}

IR Context::allocateInst(Scope *target, TypeData type, string name){
    return AllocateInst(type, VariableRegisterInst(name, type));
}

IR Context::storeInst(Scope *target, IR ptr, IR val){
    auto k = StoreInst(val, ptr);
    target->irs.push_back(k);
    return k;
}

IR Context::varInst(Scope *target, string name, TypeData type){
    auto k = VariableRegisterInst(name, type);
    return k;
}

IR Context::loadInst(Scope *target, IR value){
    auto k = LoadInst(registerInst(target), value);
    return k;
}

IR Context::accessInst(GlobalContext *gc,Scope *target, IR value, string name){
    return AccessInst(gc, value, name, registerInst(target));
}