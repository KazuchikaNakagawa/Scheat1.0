//
//  ScheatStd.hpp
//  ScheatDebugger
//
//  Created by かずちか on 2021/04/01.
//

#ifndef ScheatStd_hpp
#define ScheatStd_hpp

#include <stdio.h>
#include <vector>
#include <string>

namespace scheatSTD {
using namespace std;

struct SourceLocation {
    uint32_t line;
    uint32_t column;
    static SourceLocation OUTOFFILE;
    SourceLocation(int l, int c) : line(l), column(c) {};
    SourceLocation() = default;
    string to_string() const;
    ~SourceLocation() = default;
};

};

#endif /* ScheatStd_hpp */
