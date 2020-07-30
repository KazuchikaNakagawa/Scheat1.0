//
//  scheat.cpp
//  scheat
//
//  Created by かずちか on 2020/07/30.
//

#include <iostream>
#include "scheat.hpp"
#include "scheatPriv.hpp"

using namespace scheat;

void Scheat::HelloWorld(const char * s)
{
    scheatPriv *theObj = new scheatPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

Scheat::Scheat(int version, int section, int part, const char *target, bool debugOpt){
    this->version = version;
    this->section = section;
    this->part = part;
    this->target = target;
}

void Scheat::Debug(const char *msg, unsigned int line){
    if (!debug) {
        return;
    }
    printf("%u : %s\n", line, msg);
}

void Scheat::FatalError(const char *msg, unsigned int line){
    printf("FatalError %u : %s\n", line, msg);
    exit(0);
}

void scheatPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

