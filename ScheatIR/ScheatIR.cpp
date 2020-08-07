//
//  ScheatIR.cpp
//  scheatIRBuilder
//
//  Created by かずちか on 2020/08/06.
//

#include <stdio.h>
#include "ScheatIR.hpp"
#include "ScheatIR_Hidd.hpp"
#include <llvm/IR/Module.h>
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
#include "llvm/Support/raw_ostream.h"

#define crash printf("CrashReporter:\ncrashed in %u\n", __LINE__); exit(0)

using namespace scheatIR;
static IRContext *nowContext;
static IRContext *globalContext;

IR *IR::last(){
    IR *copy = this;
    if (copy->next == nullptr) {
        return copy;
    }
    while (copy = copy->next, copy->next != nullptr) {
        
    }
    return copy;
}

void IR::outll(std::string o){
    std::ofstream f(o);
    if (!f) {
        crash;
    }
    f << val << std::endl;
};

IR *IR::first(){
    IR *copy = this;
    if (copy->prev == nullptr) {
        return copy;
    }
    while (copy = copy->prev, copy->prev != nullptr) {
        
    }
    return copy;
}

void IRHolder::addIRToBody(IR *ir){
    body->next = ir;
    ir->prev = body;
    body = body->next;
}

void IRHolder::addIRToTail(IR *ir){
    tail->next = ir;
    ir->prev = tail;
    tail = tail->next;
}

void IRHolder::outll(std::string o){
    std::ofstream f;
    f.open(o.c_str());
    if (!f.is_open()) {
        printf("CrashReporter:\ncrashed in %u\n", __LINE__);
        exit(0);
    }
    IR *copy1 = entry;
    while (copy1 != nullptr) {
        copy1->outll(o);
        copy1 = copy1->next;
    }
    IR *copy2 = body;
    while (copy2 != nullptr) {
        copy2->outll(o);
        copy2 = copy2->next;
    }
    IR *copy3 = tail;
    while (copy3 != nullptr) {
        copy3->outll(o);
        copy3 = copy3->next;
    }
}

void IR_DefineVar::outll(std::string p){
    std::ofstream f(p);
    f << "%" << id << " = alloca " << type->ir_used << std::endl;
}

// ------------------------------------------------------------

ScheatIR::ScheatIR(std::string p) {
    path = p;
    irs = {};
    insertPoint = nullptr;
}

bool ScheatIR::exportTo(FileType ft){
    if (ft == FileType::_o) {
        return exportToMach_O();
    }
    printf("o file else is not supported yet\n");
    return false;
}

bool ScheatIR::exportToMach_O(){
    for (int i = 0;  i < irs.size(); i++) {
        irs[i].outll(path + ".ll");
    }
    llvm::LLVMContext context;
    llvm::SMDiagnostic err;
    std::unique_ptr<llvm::Module> module = llvm::parseIRFile(llvm::StringRef(path + ".ll"), err, context);
    
    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(TargetTriple);
    
    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
    
    if (!Target) {
        llvm::errs() << Error;
        return false;
    }
    
    auto CPU = "generic";
    auto Features = "";
    
    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto TheTargetMachine =
    Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);
    
    module->setDataLayout(TheTargetMachine->createDataLayout());
    
    auto Filename = "output.o";
    std::error_code EC;
    llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return false;
    }

    
    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;
    
    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        llvm::errs() << "TheTargetMachine can't emit a file of this type";
        return false;
    }
    
    pass.run(*module);
    dest.flush();
    
    return true;
    
}

void ScheatIR::addMainIR(){
    
}

InsertPoint *ScheatIR::getInsertPoint(std::string named = ""){
    if (named.empty()) {
        return insertPoint;
    }
    
}
