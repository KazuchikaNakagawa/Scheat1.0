//
//  ScheatRun.cpp
//  ScheatRun
//
//  Created by かずちか on 2020/08/10.
//

#include <iostream>
#include "ScheatRun.h"
#include "ScheatRunPriv.hpp"
#include "ScheatEngine.h"
#include <fstream>

using namespace scheatRun;

enum class InstMap : bool {
    rr = 0,
    ri = 1,
};

struct instruction::InstructionHead {
    uint8_t length;
    InstMap bitmap;
    instruction::InstructionType type;
    InstructionHead();
};

void ScheatRun::HelloWorld(const char * s)
{
    ScheatRunPriv *theObj = new ScheatRunPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void ScheatRunPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

