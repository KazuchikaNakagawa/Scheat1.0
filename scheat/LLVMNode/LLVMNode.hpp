//
//  LLVMNode.hpp
//  scheat
//
//  Created by かずちか on 2021/07/13.
//

#ifndef LLVMNode_hpp
#define LLVMNode_hpp

#include <stdio.h>
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
#include "../ScheatTypes.h"
#include "../ScheatToken.h"
#include "../ScheatContextStructures.h"
#include "../ScheatContext.h"
#include "ScheatStd.hpp"
using namespace std;
using namespace scheatSTD;
using namespace scheat;

class Node {
public:
    llvm::Type::TypeID type = llvm::Type::VoidTyID;
    SourceLocation location;
    virtual llvm::Value *codegen(unique_ptr<llvm::Module>& module,
                                 llvm::LLVMContext&,
                                 llvm::IRBuilder<> &builder
                                 );
    Node(SourceLocation l) {
        location = l;
    };
    virtual ~Node(){};
};

#endif /* LLVMNode_hpp */
