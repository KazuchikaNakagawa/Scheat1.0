//
//  scheat.hpp
//  scheat
//
//  Created by かずちか on 2020/07/30.
//

#ifndef scheat_
#define scheat_
#include <string>
#pragma GCC visibility push(default)

namespace scheat{

struct SourceLocation;

class ScheatDelegate;

class _Scheat;

class Scheat {
    bool debug;
    bool deepDebug;
    _Scheat *schobj;
    
public:
    /// returns true if this fails to compile.
    /// this is regarded as a flag.
    bool hasProbrem() const;
    
    std::string sourceFile = "";
    std::string targettingFile;
    std::string outputFilePath;
    std::string target;
    std::string datalayout;
    std::string header_search_path;
    std::string library_search_path;
    ScheatDelegate *delegate = nullptr;
    void setDebugSetting(bool o) {
        debug = o;
    };
    void allowDeepDebug(bool b){
        deepDebug = b;
    };
    Scheat();
    friend class _Scheat;
};;

class IRBuilder;

}

#pragma GCC visibility pop
#endif
