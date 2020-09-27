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
#include "YANodes.h"
#include "ScheatParser.h"
#include "ScheatContext.h"

namespace scheat {
using namespace std;
using namespace yaNodes;
namespace parser2 {

// this parser was started to build another type of parser.
// we solve type inferences, identifier existence, and so on when parsing.
unique_ptr<Statements> parse(Token *);

unique_ptr<Statement> parseStatement(Token *&);

unique_ptr<StatementNode> parseStatement_single(Token *&);

void parseExpr(Token *&);

void parsePrimary(Token *&);

void parseTerm(Token *&);

}

}

#endif /* YAScheatParser_hpp */
