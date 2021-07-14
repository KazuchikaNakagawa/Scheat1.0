//
//  LLVMIntTerm.hpp
//  scheat
//
//  Created by かずちか on 2021/07/14.
//

#ifndef LLVMIntTerm_hpp
#define LLVMIntTerm_hpp

#include <stdio.h>

#include "LLVMTerm.hpp"


class IntTerm : public Term {
    int value;
public:
    IntTerm(Token *t): Term(t->location){
        value = t->value.intValue;
        type = llvm::Type::TypeID::IntegerTyID;
    };
    llvm::Value * codegen(unique_ptr<llvm::Module> &module, llvm::LLVMContext &c, llvm::IRBuilder<> &builder) override{
        return builder.getInt32(value);
    }
};

#endif /* LLVMIntTerm_hpp */
