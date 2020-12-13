//
//  ScheatEncoder.cpp
//  scheat
//
//  Created by かずちか on 2020/10/02.
//

#include "ScheatEncoder.h"
#include "ScheatStatics.h"
#include <fstream>
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
#include <llvm/Support/raw_ostream.h>

using namespace scheat;
using namespace scheat::encoder;

void LLSCEncoder::encodeLL(_Scheat *o){
    ofstream f(o->outputFilePath + ".ll");
    for (auto cont : ScheatContext::contextCenter) {
        cont->dump(f);
    }
    f.close();
}

void LLSCEncoder::encode(_Scheat *o){
    ofstream f(o->outputFilePath + ".ll");
    for (auto cont : ScheatContext::contextCenter) {
        cont->dump(f);
    }
    f.close();
    llvm::LLVMContext context;
    llvm::SMDiagnostic err;
    unique_ptr<llvm::Module> module = llvm::parseIRFile(llvm::StringRef(o->outputFilePath + ".ll"), err, context);
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(module->getTargetTriple(), Error);
    
    if (!Target) {
        llvm::errs() << Error;
        o->FatalError(SourceLocation(), __FILE_NAME__, __LINE__,
                      "target is unclear.");
        return;
    }
    auto CPU = "generic";
    auto Features = "";
    
    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto TheTargetMachine =
    Target->createTargetMachine(module->getTargetTriple(), CPU, Features, opt, RM);
    
    module->setDataLayout(TheTargetMachine->createDataLayout());
    
    auto Filename = "output.o";
    std::error_code EC;
    llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return;
    }
    
    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;
    
    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        llvm::errs() << "TheTargetMachine can't emit a file of this type";
        return;
    }
    
    pass.run(*module);
    dest.flush();
    
    remove((o->outputFilePath + ".ll").c_str());
}

