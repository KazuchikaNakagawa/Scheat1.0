//
//  main.cpp
//  TestScheat
//
//  Created by かずちか on 2020/07/30.
//

#include <iostream>
#include <fstream>
#include <string>
#include "scheat.h"


int main(int argc, const char * argv[]) {
    //scheat::Scheat *schprj = new scheat::Scheat();
    //schprj->sourceFile = "test.scheat";
    
    //std::ofstream ofs("/Users/kaz04/ScheatDocument/test.scheat");
    //scheat::LegacyScheatParser::Parse(schprj, nullptr, ofs);
    std::string k = "ABCDEFG";
    k.erase(k.begin());
    std::cout << k << std::endl;
    return 0;
}
