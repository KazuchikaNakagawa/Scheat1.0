//
//  ScheatDebugger.hpp
//  ProjectScheat
//
//  Created by かずちか on 2021/04/01.
//

#ifndef ScheatDebugger_hpp
#define ScheatDebugger_hpp

#include <vector>
#include <string>
#include <stdio.h>
#include "ScheatInstructions.hpp"
#include "ScheatStd.hpp"

namespace schdbg {
using namespace std;
using namespace schinst;
using namespace scheatSTD;

class Debugger {
    vector<Instruction*> instmems;
public:
    static void init();
    static void deinit();
    static Instruction* createInst();
};

};
#endif /* ScheatDebugger_hpp */
