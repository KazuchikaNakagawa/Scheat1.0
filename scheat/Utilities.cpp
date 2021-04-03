//
//  Utilities.cpp
//  scheat
//
//  Created by かずちか on 2021/01/06.
//

#include <stdio.h>
#include "Utilities.h"

#include "ScheatStd.hpp"

typedef scheatSTD::SourceLocation SourceLocation;

string getFileName(string path){
    string kbuf = "";
    for (auto c : path) {
        if (c == '/') {
            kbuf = "";
            continue;
        }
        
        if (c == '.') {
            return kbuf;
        }
        
        kbuf.push_back(c);
    }
    return "";
}

