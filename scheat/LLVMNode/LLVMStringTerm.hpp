//
//  LLVMStringTerm.hpp
//  scheat
//
//  Created by かずちか on 2021/07/14.
//

#ifndef LLVMStringTerm_hpp
#define LLVMStringTerm_hpp

#include <stdio.h>
#include "LLVMTerm.hpp"
#include <string>

using namespace llvm;
class StringTerm : public Term {
    std::string value;
public:
    StringTerm(Token *tok) : Term(tok->location){
        
    }
    llvm::Value * codegen(unique_ptr<llvm::Module> &module, llvm::LLVMContext &c, llvm::IRBuilder<> &builder) override{
        // [i8 x 'n']
        llvm::ArrayType *strType = llvm::ArrayType::get(llvm::Type::getInt8Ty(c), value.size());
        // @str.0
        vector<Constant *> chars(value.size());
        for(unsigned int i = 0; i < value.size(); i++)
            chars[i] = ConstantInt::get(Type::getInt8Ty(c), value[i]);
        auto gvar = new llvm::GlobalVariable(strType, true, llvm::GlobalValue::PrivateLinkage,0,"name");
        llvm::Constant* const_hello = ConstantArray::get(strType, chars);
        ConstantInt* const_zero = ConstantInt::get(module->getContext(), APInt(32, StringRef("0"), 10));
        std::vector<Constant*> constants;
        constants.push_back(const_zero);
        constants.push_back(const_zero);
        Constant* const_ptr = ConstantExpr::getGetElementPtr(strType, const_hello, builder.getInt32(0));
        gvar->setInitializer(const_hello);
        return const_ptr;
    };
};

#endif /* LLVMStringTerm_hpp */
