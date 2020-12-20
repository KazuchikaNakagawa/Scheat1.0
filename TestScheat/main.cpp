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

using namespace scheat;
int main(int argc, const char * argv[]) {
    Scheat scheat = Scheat();
    scheat.setDebugSetting(true);
    scheat.allowDeepDebug(true);
    scheat.datalayout = "; testlayout";
    scheat.ready();
    ScheatLexer::testlex("this b is 8 + 8.");
    if (scheat.hasProbrem()) {
        return 0;
    }
    ScheatAnalyzer::parse();
    if (scheat.hasProbrem()) {
        return 0;
    }
    ScheatEncoder::printout();
    return 0;
}

