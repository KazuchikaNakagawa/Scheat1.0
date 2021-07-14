//
//  LLVMStatementNode.hpp
//  scheat
//
//  Created by かずちか on 2021/07/13.
//

#ifndef LLVMStatementNode_hpp
#define LLVMStatementNode_hpp

#include <stdio.h>
#include "LLVMNode.hpp"

class StatementNode : public Node {
    
public:
    StatementNode(SourceLocation l): Node(l){
        
        
    }
    virtual ~StatementNode(){};
    virtual llvm::Value * codegen(unique_ptr<llvm::Module> &module, llvm::LLVMContext &, llvm::IRBuilder<> &builder) override{
        return nullptr;
    };
};

#endif /* LLVMStatementNode_hpp */
