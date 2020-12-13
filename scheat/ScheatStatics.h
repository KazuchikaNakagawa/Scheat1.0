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
#include "ScheatContext.h"
#include "ScheatToken.h"
#include "ScheatObjects.h"

namespace scheat {


extern std::map<int, std::vector<std::string>> objects;
extern std::string fname;
extern Token *mTokens;
extern Scheat *scheato;


}

#endif /* ScheatParser_h */
