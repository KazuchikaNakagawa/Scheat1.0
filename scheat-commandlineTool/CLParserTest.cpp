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
int main(int argc,const char *argv[]){
    OptionStream options(argc, argv);
    options.parse();
    options.printBuffer();
    if (options.isIncluded("-b")) {
        printf("waaaao\n");
    }
    auto aopt = options.getOption("-d", type_string);
    if (!aopt) {
        printf("no args such name and args.\n");
    }else{
        printf("%s %s\n", aopt->name.c_str(), aopt->value[0].data.svalue);
    }
    auto bopt = options.getOption("-d", type_string);
    if (!bopt) {
        printf("no args such name and args.\n");
    }else{
        printf("%s %s\n", bopt->name.c_str(), bopt->value[0].data.svalue);
    }
    return 0;
}
