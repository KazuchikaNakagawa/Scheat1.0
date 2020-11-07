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
    schprj->allowDeepDebug(true);
    scheat::statics::scheato = schprj;
    //scheat::LegacyScheatParser::LLParse(schprj);
    scheat::lexer::Lexer lxr(schprj);
    scheat::InitializeAll();
    scheat::InitializeFoundationClass();
    lxr.lex("1 * 2 - 78");
    auto tokens = lxr.getTokens();
    auto ptr = scheat::parser2::parseExpr(tokens);
    scheat::IRStream f;
    ptr->codegen(f);
    f.printout();
    return 0;
}

