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

void Instance::dump(ostream &f){
    f << "instance at " << hex << properties.begin()->value.i64Ptr << ":\n";
    for (int i = 0; i < properties.size(); i++) {
        f << "    ";
        if (properties[i].size == instance) {
            properties[i].value.dump(f);
        }else{
            properties[i].value.dump(f);
        }
        f << "\n";
    }
}

uint64_t Instance::calcSize(){
    uint64_t size = 0;
    for (int i = 0; i < properties.size(); i++) {
        if (properties[i].size == instance) {
            size += properties[i].value.instance.calcSize();
        }else{
            size += properties[i].size;
        }
    }
    return size;
}

void Value::dump(ostream &f){
    
}

//void log(uint64_t adrs, ostream &o){
//    printf("%#llx (%lldbit): ", (uint64_t)buffer, size);
//    for (int i = 0; i < size; i++) {
//        printf("%d", buffer[i]);
//    }
//    printf("\n");
//}
