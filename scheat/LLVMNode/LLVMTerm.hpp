//
//  LLVMTerm.hpp
//  scheat
//
//  Created by かずちか on 2021/07/13.
//

#ifndef LLVMTerm_hpp
#define LLVMTerm_hpp

#include <stdio.h>
#include "LLVMPrimaryExpr.hpp"

class Term : public PrimaryExpr {
    
public:
    virtual llvm::Value * codegen(unique_ptr<llvm::Module> &module, llvm::LLVMContext &, llvm::IRBuilder<> &builder) override{
        return nullptr;
    };
    Term(SourceLocation l) : PrimaryExpr(l){};
};

#endif /* LLVMTerm_hpp */
