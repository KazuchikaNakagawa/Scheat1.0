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
#include "ScheatStatics.h"

namespace scheat {

namespace statics {

using namespace scheat;
using namespace scheat::statics;
using namespace scheat::basics;

std::vector<Context *> contextCenter = {};
Context *global_context = nullptr;
Context *main_Context = nullptr;
std::stack<Context *> local_context = {};
std::map<int, std::vector<std::string>> objects = {};
std::string fname = "";
Token *mTokens = nullptr;
Scheat *scheato = nullptr;
IRStream initStream;
}

}
