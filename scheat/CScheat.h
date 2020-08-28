//
//  CScheat.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/28.
//

#ifndef CScheat_h
#define CScheat_h

#include "scheat.h"
/// this file eill enable to use Scheat in C.
/// if so, you can use in Objective-C.
/// and Swift will be able to use this.
extern "C" bool scheat_build(const char *outputPath, const char *option, const char *sourceFilePath, const char *headerSearchPath, const char *librarySearchPath);

#endif /* CScheat_h */
