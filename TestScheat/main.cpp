//
//  main.cpp
//  TestScheat
//
//  Created by かずちか on 2020/07/30.
//

#include <iostream>
#include "scheat.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    scheat::Scheat sch = scheat::Scheat();
    sch.FatalError("Use this as a test.", __LINE__);
    printf("this is iMac text;\n");
    return 0;
}
