//
//  Classes.h
//  ProjectScheat
//
//  Created by かずちか on 2020/10/02.
//

#ifndef Classes_h
#define Classes_h
#include "ScheatObjects.h"
#include <string>

namespace scheat {
//using scheat::Token;
class _Scheat;
class Token;

class ScheatLexer {
public:
    static void lex();
    static void testlex(std::string);
};

class ScheatAnalyzer {
public:
    static void parse();
};

class ScheatEncoder {
public:
    static void encode();
    static void printout();
};


}

#endif /* Classes_h */
