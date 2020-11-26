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
#include "ScheatBasicStructures.h"
#include "ScheatContext.h"

namespace scheat {

namespace statics {

extern std::map<int, std::vector<std::string>> objects;
extern std::string fname;
extern scheat::Token *mTokens;
extern Scheat *scheato;
extern IRStream initStream;

class ScheatContext {
    static stack<Context *> localcon;
public:
    static vector<Context *> contextCenter;
    static void Init(Scheat *);
    static void AddMain();
    static void Shutdown(){ delete ScheatContext::global; };
    static Context *main;
    static Context *global;
    static Context *init;
    static Context *local();
    static void push(Context *c) { localcon.push(c); };
    static void pop() { localcon.pop(); };
};

}

}

#endif /* ScheatParser_h */
