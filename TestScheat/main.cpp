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
    scheat::Scheat *schprj = new scheat::Scheat();
    schprj->sourceFile = "test.scheat";
    
    std::ofstream ofs("/Users/kaz04/ScheatTest/test.scheat");
    if (!ofs.is_open()) {
        schprj->FatalError("", 0, "File not found");
        return 0;
    }
    scheat::LegacyScheatParser::Parse(schprj, nullptr, ofs);
    
    return 0;
}
