//
//  LLVMPrimaryExpr.hpp
//  scheat
//
//  Created by かずちか on 2021/07/13.
//

#ifndef LLVMPrimaryExpr_hpp
#define LLVMPrimaryExpr_hpp

#include <stdio.h>
#include "LLVMExpr.hpp"

class PrimaryExpr : public Expr {
    
public:
    virtual llvm::Value * codegen(unique_ptr<llvm::Module> &module, llvm::LLVMContext &, llvm::IRBuilder<> &builder) override{
        return nullptr;
    };
    PrimaryExpr(SourceLocation l) : Expr(l){};
};

#endif /* LLVMPrimaryExpr_hpp */
