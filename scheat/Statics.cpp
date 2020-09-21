//
//  Statics.cpp
//  scheat
//
//  Created by かずちか on 2020/09/13.
//

#include <stdio.h>
#include <vector>
#include <stack>
#include <map>
#include <string>
#include "ScheatNodes.h"
#include "ScheatContext.h"
#include "ScheatParser.h"

namespace scheat {

namespace parser {

using namespace scheat;
using namespace scheat::parser;
using namespace scheat::basics;

std::vector<Context *> contextCenter = {};
Context *global_context = nullptr;
Context *main_Context = nullptr;
std::stack<Context *> local_context = {};
std::map<int, std::vector<std::string>> objects = {};
std::string fname = "";
Token *mTokens = nullptr;
Scheat *scheato = nullptr;

}

}
