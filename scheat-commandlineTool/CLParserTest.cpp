//
//  CLParserTest.cpp
//  CLParserTest
//
//  Created by かずちか on 2021/01/18.
//

#include <stdio.h>
#include "CommandLineOptions.hpp"
#include <string>

using namespace commandLine;
using namespace std;
int main(){
    const char *ptrs[1] = {(char*)"-a 12 -c -b lp"};
    OptionStream options(1, ptrs);
    options.parse();
    if (options.isIncluded("-b")) {
        printf("waaaao\n");
    }
    auto aopt = options.getOption("-c", type_no_args);
    if (!aopt) {
        printf("no args such nameand args.\n");
    }else{
        printf("%s %s\n", aopt->name.c_str(), aopt->value[0].data.svalue);
    }
    return 0;
}
