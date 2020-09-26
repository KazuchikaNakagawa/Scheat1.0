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
    K s;
    s.cp = std::string("asjkj").c_str();
    print_String(&s.s);
    return 0;
}
