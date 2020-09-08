//
//  LegacyTypeScheat.hpp
//  scheat
//
//  Created by かずちか on 2020/08/09.
//

#ifndef LegacyTypeScheat_hpp
#define LegacyTypeScheat_hpp

#include <stdio.h>
#include "ScheatObjects.h"
#include "Lexer.hpp"
#include "ScheatNodes.h"
#include <fstream>

namespace scheat{

namespace LegacyScheatParser {

namespace E9 {

void InitializeContexts();

void CreateMainContext();

}

void LLParse(Scheat *);

std::vector<std::unique_ptr<node::Statements>> Parse(Scheat *);

}


}
#endif /* LegacyTypeScheat_hpp */
