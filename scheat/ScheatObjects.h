//
//  ScheatObjects.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/28.
//

#ifndef ScheatObjects_h
#define ScheatObjects_h
#include <string>
namespace scheat {

struct SourceLocation {
    int line;
    int column;
    SourceLocation(){
        line = 1;
        column = 0;
    }
    ~SourceLocation() = default;
};

struct DebugOption {
    bool enableDebug;
    bool developerMode;
};

class Scheat {
    bool debug;
    bool deepDebug;
public:
    SourceLocation location;
    std::string targettingFile;
    std::string outputFilePath;
    std::string target;
    std::string datalayout;
    std::string header_search_path;
    std::string library_search_path;
    void debugSet(bool o) {
        debug = o;
    };
    void allowDeepDebug(bool b){
        deepDebug = b;
    };
    void FatalError(const char *, unsigned int, const char *, ...) ;
    void Log(const char *,unsigned int, const char *, ...);
    void Warning(const char *,unsigned int, const char *, ...);
    void DevLog(const char *,unsigned int, const char *, ...);
    Scheat();
};

}

#endif /* ScheatObjects_h */
