//
//  main.cpp
//  ScheatARCTest
//
//  Created by かずちか on 2020/08/27.
//

#include <iostream>
#include "ScheatCFoundation.h"

void notify(void *p){
    printf("%llx is released\n", uint64_t(p));
}

static char *ptr = nullptr;

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    char *h = (char *)ScheatPointer_alloc(sizeof(char) * 8, notify);
    sprintf(h, "1234567");
    char *i = (char *)ScheatPointer_copy(h);
    ScheatPointer_unref(h);
    printf("%s\n", h);
    printf("%s\n", i);
    ptr = (char *)ScheatPointer_copy(i);
    ScheatPointer_unref(i);
    printf("%s\n", i);
    ScheatPointer_unref(ptr);
    return 0;
}
