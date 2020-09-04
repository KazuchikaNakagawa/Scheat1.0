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
    schprj->outputFilePath = "/Users/kaz04/ScheatTest/test";
    
    scheat::Token *toks = scheat::Lexer::lex(schprj);
    scheat::LegacyScheatParser::Parse(schprj, toks);
    
    return 0;
}
