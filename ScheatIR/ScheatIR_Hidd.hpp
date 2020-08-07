//
//  ScheatIR_Hidd.hpp
//  ProjectSpica
//
//  Created by かずちか on 2020/08/06.
//

#ifndef ScheatIR_Hidd_h
#define ScheatIR_Hidd_h
#pragma GCC visibility push(hidden)
#include <iostream>
#include <string>
#include <map>

namespace scheatIR {

class IRContext {
    // in almost all case, it will be empty.
    // this will be filled only in  globalContext
    std::map<std::string, TypeData *> types;
    
    // this has variable data.
    std::map<std::string, Variabledata *> variables;
public:
    TypeData *getType(std::string key, bool forced = false);
    Variabledata *getVar(std::string key, bool forced = false);
    void addType(TypeData *);
};

}

#pragma GCC visibility pop
#endif /* ScheatIR_Hidd_h */
