//
//  ScheatInstructions.hpp
//  ScheatDebugger
//
//  Created by かずちか on 2021/04/01.
//

#ifndef ScheatInstructions_hpp
#define ScheatInstructions_hpp

#include <stdio.h>
#include "ScheatStd.hpp"
namespace schinst {
using namespace scheatSTD;
using namespace std;
struct ASMValue {
    string value;
    uint32_t size;
    ASMValue(string s, uint32_t l){
        value = s;
        size = l;
    }
    static ASMValue voidValue;
};

class Instruction {
public:
    SourceLocation location;
    virtual void execute();
    virtual ASMValue encodeASM();
    virtual ASMValue encodeBinary();
    virtual ASMValue encodeScheatBinary();
};

// push rsp
class CreateLocalVariableInst;

class CreateGlobalVariableInst;



}

#endif /* ScheatInstructions_hpp */
