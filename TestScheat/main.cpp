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
    /*
     target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
     target triple = "x86_64-apple-macosx10.14.0"
     */
    scheat.target = "x86_64-apple-macosx10.15.0";
    scheat.datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128";
    scheat.isMain = false;
    scheat.setProductName("testProduct");
    scheat.outputFilePath = "/Users/kaz04/ScheatDocument/XCODETEST.ll";
    scheat.sourceFile = "/Users/kaz04/ScheatDocument/XCODETEST.scheat";
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
    //ScheatEncoder::encode();
    return 0;
}

