//
//  ScheatObjects.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/28.
//

#ifndef ScheatObjects_h
#define ScheatObjects_h
#include <string>
#include <vector>

#define SV_P 14
namespace scheat {
class Token;
class Scheat;
namespace LegacyScheatParser {
extern void LLParse(Scheat *);
}

using namespace std;

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

class ScheatDelegate {
public:
    
    virtual void fatalError(Scheat *_scheat,SourceLocation location, std::string filePath, std::string message, ...);
    
    virtual void warning(Scheat *_scheat, SourceLocation location, std::string filePath, std::string message, ...);
    
    virtual void log(Scheat *_scheat, SourceLocation location, std::string filePath, std::string message, ...);
    
    virtual std::string target_triple();
    
    virtual std::string datalayout();
    
};

namespace yaNodes{

class DataHolder;

};

using namespace yaNodes;

/// represents a project file like CMakeLists.txt
class Scheat {
    bool debug;
    bool deepDebug;
    bool hasError;
    scheat::Token *tokens;
    
public:
    /// returns true if this fails to compile.
    /// this is regarded as a flag.
    bool hasProbrem() const { return hasError; };
    
    /// location to be compiled
    SourceLocation location;
    
    std::string sourceFile = "";
    std::string targettingFile;
    std::string outputFilePath;
    std::string target;
    std::string datalayout;
    std::string header_search_path;
    std::string library_search_path;
    ScheatDelegate *delegate = nullptr;
    DataHolder *statements = nullptr;
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
    
    friend class Lexer;
    friend void LegacyScheatParser::LLParse(Scheat *);
};

enum class ScheatError : int {
    ERR_node_has_illegal_value = -1,
    ERR_UNKNOWN = 0,
};

/// deprecated
/// Scheat needs this object to move.
class OldScheat
{
    /// version.section.part
    /// 1.0.0
    int version = 1;
    
    int section = 0;
    
    int part = SV_P;
    
    const char *target = "UNKNOWN TARGET";
    
    bool debug = false;
    OldScheat(int version = 1, int section = 0, int part = SV_P, const char *target = "", bool debugOpt = false);
public:
    
    // initialize with string that shows version.
    OldScheat(const char *format, const char *target = "", bool debugOpt = false);
    __unavailable
    void HelloWorld(const char *);
    // this function was deprecated because new function with varg is there instead of this.
    __deprecated void old_FatalError(const char *, unsigned int);
    void FatalError(const char *, unsigned int, const char *, ...) ;
    __deprecated void old_Debug(const char *, unsigned int);
    void Log(const char *,unsigned int, const char *, ...);
    void flagDebug() { debug = !debug; };
    void printVersion();
    int getVersion() const { return version; };
    int getSection() const { return section; };
    int getPart() const { return part; };
    
    static OldScheat& shared(){
        static OldScheat _scheat(1,0,SV_P);
        return _scheat;
    };
};
}

#endif /* ScheatObjects_h */
