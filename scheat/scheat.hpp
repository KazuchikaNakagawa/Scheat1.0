//
//  scheat.hpp
//  scheat
//
//  Created by かずちか on 2020/07/30.
//

#ifndef scheat_
#define scheat_

/* The classes below are exported */
#pragma GCC visibility push(default)

namespace scheat{

class Scheat
{
    /// version.section.part
    /// 1.0.0
    int version;
    int section;
    int part;
    const char *target;
    bool debug;
    public:
    Scheat(int version = 1, int section = 0, int part = 0, const char *target = "", bool debugOpt = false);
    void HelloWorld(const char *);
    void FatalError(const char *, unsigned int);
    void Debug(const char *, unsigned int);
};

class Statement;

class Expression;

class IntExpr;

class StringExpr;

class BoolExpr;

class DoubleExpr;

class IdentifierExpr;

class IRBuilder{
public:
    
};

}

#pragma GCC visibility pop
#endif
