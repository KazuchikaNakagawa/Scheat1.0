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
#include "ScheatParser2.h"
#include "Initializers.h"
#include "ScheatEncoder.h"
#pragma GCC visibility push(default)

namespace scheat{

struct SourceLocation;

class ScheatDelegate;

class Scheat;

class IRBuilder;

extern void LegacyScheatParser::LLParse(Scheat *);

extern void parser2::parse(Scheat *, Token *);

}

#pragma GCC visibility pop
#endif
