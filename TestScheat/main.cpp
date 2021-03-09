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
    scheat.logInfo(true);
    scheat.logAllInfo(true);
    /*
     target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
     target triple = "x86_64-apple-macosx10.14.0"
     */
//    scheat.target = "x86_64-apple-macosx10.15.0";
//    scheat.datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128";
    scheat.isMain = true;
    
    scheat.sourceFile = "/Users/kaz04/ScheatDocument/XCODETEST.scheat";
    scheat.deletesIRFiles = false;
    scheat.complementSettings();
    scheat.ready();
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
    //system("clang++ -o HW HelloWorld.o -L/usr/local/lib/Scheat -lFoundation -v");
    return 0;
}

