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
    scheat.datalayout = "i32-SCHEAT64";
    scheat.isMain = true;
    scheat.setProductName("testProduct");
    scheat.outputFilePath = "/Users/kaz04/ScheatDocument/XCODETEST.ll";
    scheat.sourceFile = "/Users/kaz04/ScheatDocument/XCODETEST.scheat";
    scheat.ready();
    
    //printf("this b is \"assass\".::::\nlog b.");
    ScheatLexer::lex();
    if (scheat.hasProbrem()) {
        return 0;
    }
    ScheatAnalyzer::parse();
    if (scheat.hasProbrem()) {
        return 0;
    }
    ScheatEncoder::printout();
    ScheatEncoder::encode();
    return 0;
}

