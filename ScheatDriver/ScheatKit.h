//
//  ScheatKit.h
//  ProjectScheat
//
//  Created by かずちか on 2020/12/16.
//

#ifndef ScheatKit_h
#define ScheatKit_h
#include <string>

namespace scheat {

using namespace std;

enum class MessageType : int {
    error = -1,
    warning = 0,
    log = 1
};

struct Message{
    SourceLocation location;
    string fileName;
    MessageType type;
};

extern void executeScheat(string targetFilePath,
                   string productName,
                   string headerSearchPath,
                   string librarySearchPath,
                   bool debugs = true,
                   bool debugDetails = false,
                   string targetName = "",
                   string datalayout = "");

}

#endif /* ScheatKit_h */
