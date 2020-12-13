//
//  BasicStructures.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/26.
//

#ifndef BasicStructures_h
#define BasicStructures_h

#include <map>
#include <vector>
#include "ScheatObjects.h"
#include "ScheatToken.h"
#include "ScheatTypes.h"
#include "ScheatContextStructures.h"
#include <stack>
namespace scheat {

class Context;

namespace statics{
class ScheatContext {
    static stack<Context *> localcon;
public:
    static vector<Context *> contextCenter;
    static void Init(Scheat *);
    static void AddMain();
    static void Shutdown(){ //delete ScheatContext::global;
        
    };
    static Context *main;
    static Context *global;
    static Context *init;
    static Context *local();
    static void push(Context *c) { localcon.push(c); };
    static void pop() { localcon.pop(); };
    static void printout();
};
}

}

#endif /* BasicStructures_h */
