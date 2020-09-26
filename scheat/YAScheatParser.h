//
//  YAScheatParser.hpp
//  scheat
//
//  Created by かずちか on 2020/09/26.
//

#ifndef YAScheatParser_hpp
#define YAScheatParser_hpp

#include <stdio.h>
#include <vector>
#include <stack>
#include <map>
#include <string>
#include "ScheatNodes.h"
#include "YANodes.h"
#include "ScheatContext.h"

namespace scheat {
using namespace std;
namespace parser2 {

// this parser was started to build another type of parser.
// we solve type inferences, identifier existence, and so on when parsing.
unique_ptr<node::Statements> parseStatements(Token *);

unique_ptr<node::StatementNode> parseStatement(Token *&);

void parseExpr(Token *&);

void parsePrimary(Token *&);

void parseTerm(Token *&);

}

}

#endif /* YAScheatParser_hpp */
