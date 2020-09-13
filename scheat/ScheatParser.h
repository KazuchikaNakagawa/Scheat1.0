//
//  ScheatParser.h
//  ProjectScheat
//
//  Created by かずちか on 2020/09/13.
//

#ifndef ScheatParser_h
#define ScheatParser_h

#include <vector>
#include <stack>
#include <map>
#include <string>
#include "ScheatNodes.h"
#include "ScheatContext.h"

namespace scheat {

namespace parser {

static std::vector<Context *> contextCenter = {};
static Context *global_context;
static Context *main_Context;
static std::stack<Context *> local_context;
static std::map<int, std::vector<std::string>> objects;
static std::string fname = "";

}

}

#endif /* ScheatParser_h */
