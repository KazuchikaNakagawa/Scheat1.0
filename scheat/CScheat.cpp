//
//  CScheat.cpp
//  scheat
//
//  Created by かずちか on 2020/08/28.
//

#include <stdio.h>
#include "CScheat.h"
#include "ScheatObjects.h"
#include "Lexer.hpp"

using namespace scheat;
using std::string;

bool scheat_build(const char *outputPath, const char *option, const char *sourceFilePath, const char *headerSearchPath, const char *librarySearchPath){
    printf("this feture is not available yet.\n");
    // exit(0);
    _Scheat *scheatprj = new _Scheat();
    scheatprj->sourceFile = std::string(sourceFilePath);
    scheatprj->header_search_path = std::string(headerSearchPath);
    scheatprj->library_search_path = string(librarySearchPath);
    lexer::Lexer::lexThis(scheatprj);
    if (scheatprj->hasProbrem()) {
        return false;
    }
    
    return false;
}
