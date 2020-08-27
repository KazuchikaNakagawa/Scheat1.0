//
//  ScheatEngine.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/27.
//

#ifndef ScheatEngine_h
#define ScheatEngine_h

#include <stack>

using std::stack;
using std::bitset;

namespace scheatRun {

namespace engine {

class ScheatRunEngine {
public:
    stack<bitset<8>> stacks;
    
};

}

}

#endif /* ScheatEngine_h */
