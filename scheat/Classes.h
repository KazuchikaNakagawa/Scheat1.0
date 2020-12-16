//
//  Classes.h
//  ProjectScheat
//
//  Created by かずちか on 2020/10/02.
//

#ifndef Classes_h
#define Classes_h
#include "ScheatObjects.h"

namespace scheat {
//using scheat::Token;
class _Scheat;
class Token;

class ScheatLexer {
public:
    static void lex();
};

class ScheatAnalyzer {
public:
    static void parse();
};

class ScheatEncoder {
public:
    static void encode();
};


}

#endif /* Classes_h */
