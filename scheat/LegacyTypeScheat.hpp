//
//  LegacyTypeScheat.hpp
//  scheat
//
//  Created by かずちか on 2020/08/09.
//

#ifndef LegacyTypeScheat_hpp
#define LegacyTypeScheat_hpp

#include <stdio.h>
#include "scheat.hpp"
#include <fstream>

namespace LegacyScheat {

namespace E9 {

void InitializeContexts();

void CreateMainContext();

}

void Parse(scheat::Token *tokens, std::ofstream&);

}



#endif /* LegacyTypeScheat_hpp */
