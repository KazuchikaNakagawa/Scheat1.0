//
//  ScheatRun.cpp
//  ScheatRun
//
//  Created by かずちか on 2020/08/10.
//

#include <iostream>
#include "ScheatRun.hpp"
#include "ScheatRunPriv.hpp"
#include <fstream>

using namespace ScheatRunSystem;

enum class InstMap : bool {
    rr = 0,
    ri = 1,
};

struct ScheatRunSystem::InstructionHead {
    std::bitset<8> length;
    InstructionType type;
    template <unsigned char Size>
    Instruction<Size> *makeInst(std::ifstream &);
    InstructionHead();
};

void ScheatRunSystem::ScheatRun::HelloWorld(const char * s)
{
    ScheatRunPriv *theObj = new ScheatRunPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void ScheatRunPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

