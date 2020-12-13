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
    schprj->setDebugSetting(true);
    scheat::statics::scheato = schprj;
    //scheat::LegacyScheatParser::LLParse(schprj);
    scheat::lexer::Lexer lxr(schprj);
    //scheat::InitializeAll();
    //scheat::InitializeFoundationClass();
    lxr.lex("this i is 7 + 8 + 2. ");
    auto tokens = lxr.getTokens();
    tokens->enumerate();
    scheat::statics::ScheatContext::Init(schprj);
    scheat::statics::ScheatContext::AddMain();
    
    auto ptr = scheat::parser2::parseStatement(tokens);
    
    ptr->codegen(scheat::statics::ScheatContext::main->stream_body);
    scheat::statics::ScheatContext::printout();
    return 0;
}

