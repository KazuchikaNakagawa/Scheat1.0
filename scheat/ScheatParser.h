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

extern std::vector<Context *> contextCenter;
extern Context *global_context;
extern Context *main_Context;
extern std::stack<Context *> local_context;
extern std::map<int, std::vector<std::string>> objects;
extern std::string fname;
extern scheat::Token *gltokens;
extern Scheat *scheato;
}

}

#endif /* ScheatParser_h */
