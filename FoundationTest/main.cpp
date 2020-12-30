//
//  main.cpp
//  FoundationTest
//
//  Created by かずちか on 2020/09/26.
//

#include <iostream>
#include <stdio.h>
#include "ScheatCFoundation.h"
#include <string>

union K {
    const char *cp;
    String s;
};

int main(int argc, const char * argv[]) {
    // insert code here...
    Array arr = Array_init(sizeof(bool));
    bool c = true;
    Array_append(&arr, &c);
    c = false;
    Array_append(&arr, &c);
    c = true;
    Array_append(&arr, &c);
    c = false;
    Array_append(&arr, &c);
    for (int i = 0; i < arr.count; i++) {
        auto p = *(bool *)Array_at(&arr, i);
        printf("%d", p);
    }
    return 0;
}
