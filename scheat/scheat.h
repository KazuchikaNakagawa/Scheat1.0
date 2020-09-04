//
//  scheat.hpp
//  scheat
//
//  Created by かずちか on 2020/07/30.
//

#ifndef scheat_
#define scheat_

#include "Lexer.hpp"
#include "ScheatObjects.h"
#include "LegacyTypeScheat.hpp"
#pragma GCC visibility push(default)

namespace scheat{

struct SourceLocation;

class ScheatDelegate;

class Scheat;

class IRBuilder;

extern void LegacyScheatParser::Parse(Scheat *, scheat::Token *tokens);

}

#pragma GCC visibility pop
#endif
