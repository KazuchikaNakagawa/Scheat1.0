//
//  ScheatKit.cpp
//  ScheatDriver
//
//  Created by かずちか on 2020/12/16.
//

#include <stdio.h>
#include "scheat.h"
#include "ScheatKit.h"
#include <string>

using namespace std;

void scheat::executeScheat(string targetFilePath, string productName, string headerSearchPath, string librarySearchPath, bool debugs, bool debugDetails, string targetName, string datalayout){
    Scheat scheat = Scheat();
    scheat.header_search_path = headerSearchPath;
    scheat.logAllInfo(debugDetails);
    scheat.logInfo(debugs);
    scheat.library_search_path = librarySearchPath;
    scheat.outputFilePath = productName;
    scheat.datalayout = datalayout;
    scheat.target = targetName;
    scheat.sourceFile = targetFilePath;
    scheat.ready();
    ScheatLexer::lex();
    if (scheat.hasProbrem()) {
        return;
    }
    ScheatAnalyzer::parse();
    if (scheat.hasProbrem()) {
        return;
    }
    ScheatEncoder::encode();
}
