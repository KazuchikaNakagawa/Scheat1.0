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
#include "Nodes2.h"
#include "ScheatStatics.h"
#include "ScheatContext.h"

namespace scheat {
using namespace std;
using namespace nodes2;
namespace parser2 {

// this parser was started to build another type of parser.
// we solve type inferences, identifier existence, and so on when parsing.
void parse(Scheat *,Token *);

unique_ptr<Statement> parseStatement(Token *&);

unique_ptr<StatementNode> parseStatement_single(Token *&);

unique_ptr<Expr> parseExpr(Token *&);

unique_ptr<OperatedPrimaryExpr> parsePrimary(Token *&);

unique_ptr<Term> parseTerm(Token *&);

}

}

#endif /* YAScheatParser_hpp */
