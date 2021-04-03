//
//  ScheatInstructions.cpp
//  ScheatDebugger
//
//  Created by かずちか on 2021/04/01.
//

#include "ScheatInstructions.hpp"

using namespace scheatSTD;
using namespace schinst;

ASMValue ASMValue::voidValue = ASMValue("", 0);

class CreateLocalVariableInst : public Instruction {
    
public:
    ASMValue encodeASM() override{
        return ASMValue::voidValue;
    }
};
