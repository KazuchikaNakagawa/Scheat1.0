//
//  main.cpp
//  TestScheat
//
//  Created by かずちか on 2020/07/30.
//

#include <iostream>
#include "scheat.hpp"


int main(int argc, const char * argv[]) {
    // insert code here...
    scheat::Scheat sch = scheat::Scheat();
    sch.flagDebug();
    sch.Debug(__LINE__, "Hello");
    scheat::Lexer lexer = scheat::Lexer(&sch);
    lexer.lex("this is unko.");
    lexer.getTokens()->enumerate();
    return 0;
}
