//
//  main.cpp
//  TestScheat
//
//  Created by かずちか on 2020/07/30.
//

#include <iostream>
#include "scheat.hpp"


int main(int argc, const char * argv[]) {
    scheat::Scheat sch = scheat::Scheat();
    sch.flagDebug();
    sch.Log(__FILE_NAME__,__LINE__, "Hello");
    scheat::Lexer lexer = scheat::Lexer(&sch);
    lexer.lex("\tnani are (1,500!).");
    lexer.getTokens()->enumerate();
    return 0;
}
