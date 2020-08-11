//
//  ScheatRun.cpp
//  ScheatRun
//
//  Created by かずちか on 2020/08/10.
//

#include <iostream>
#include "ScheatRun.hpp"
#include "ScheatRunPriv.hpp"

class ScheatRunSystem::Instruction {
    
public:
    
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

