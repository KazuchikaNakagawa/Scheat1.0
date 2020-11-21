//
//  ProjectPlebs.cpp
//  scheat
//
//  Created by かずちか on 2020/11/21.
//

#include <stdio.h>
#include "ProjectPlebs.h"
#include <iostream>
#include <bitset>

using namespace std;
using namespace plebs;

void Instance::log(uint64_t adrs, ostream &o){
    string buf = "instance at " + to_string(adrs) + ":\n";
    o << buf;
    uint64_t add = adrs;
    for (auto b : sizes) {
        o << "    ";
        b.log(add, o);
        add += b.size;
    }
    
}

void Buffer::log(uint64_t adrs, ostream &o){
    
}
