//
//  LLVMExpr.hpp
//  scheat
//
//  Created by かずちか on 2021/07/13.
//

#ifndef LLVMExpr_hpp
#define LLVMExpr_hpp

#include <stdio.h>
#include "LLVMNode.hpp"

class Expr : public Node {
    
public:
    virtual llvm::Value * codegen(unique_ptr<llvm::Module> &module, llvm::LLVMContext &, llvm::IRBuilder<> &builder) override{
        return nullptr;
    }
    Expr(SourceLocation l): Node(l){};
};

#endif /* LLVMExpr_hpp */
