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
        column = 1;
    }
    ~SourceLocation() = default;
};
class Scheat {
    bool debug;
    bool developerMode;
    std::string targettingFile;
    SourceLocation location;
public:
    std::string outputFilePath;
    std::string target;
    std::string datalayout;
    std::string header_search_path;
    std::string library_search_path;
    void debugSet(bool o) {
        debug = o;
    };
    void developerModeSet(bool b){
        developerMode = b;
    };
    void FatalError(const char *, unsigned int, const char *, ...) ;
    void Log(const char *,unsigned int, const char *, ...);
    void Warning(const char *,unsigned int, const char *, ...);
    void DevLog(const char *,unsigned int, const char *, ...);
    Scheat();
};

}

#endif /* ScheatObjects_h */
