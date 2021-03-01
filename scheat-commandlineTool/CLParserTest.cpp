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
    auto UTSOption = options.getOption("-enableFastLog", type_no_args);
    if (UTSOption) {
        printf("found!\n");
    }
    return 0;
}
