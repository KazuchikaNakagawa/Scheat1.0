//
//  scheat.hpp
//  scheat
//
//  Created by かずちか on 2020/07/30.
//

#ifndef scheat_
#define scheat_

#pragma GCC visibility push(default)
// #include <string>
#include "Lexer.hpp"
#include <string>

#define SV_P 7

namespace scheat{

enum class ScheatError : int {
    ERR_node_has_illegal_value = -1,
    ERR_UNKNOWN = 0,
};

// DO NOT ACCESS TO THIS
static bool DEBUGOPTION = false;

/// Scheat needs this object to move.
class Scheat
{
    /// version.section.part
    /// 1.0.0
    int version = 1;
    
    int section = 0;
    
    int part = SV_P;
    
    const char *target = "UNKNOWN TARGET";
    
    bool debug = false;
    Scheat(int version = 1, int section = 0, int part = SV_P, const char *target = "", bool debugOpt = false);
public:
    
    // initialize with string that shows version.
    Scheat(const char *format, const char *target = "", bool debugOpt = false);
    __unavailable
    void HelloWorld(const char *);
    // this function was deprecated because new function with varg is there instead of this.
    __deprecated void old_FatalError(const char *, unsigned int);
    void FatalError(const char *, unsigned int, const char *, ...) ;
    __deprecated void old_Debug(const char *, unsigned int);
    void Log(const char *,unsigned int, const char *, ...);
    void flagDebug() { debug = !debug; DEBUGOPTION = debug; };
    void printVersion();
    int getVersion() const { return version; };
    int getSection() const { return section; };
    int getPart() const { return part; };
    
    static Scheat& shared(){
        static Scheat _scheat(1,0,SV_P);
        return _scheat;
    };
};

// Scheat System Key
class Scheat_ {
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
    void debugSet(bool);
    void FatalError(const char *, unsigned int, const char *, ...) ;
    void Log(const char *,unsigned int, const char *, ...);
    void Warning(const char *,unsigned int, const char *, ...);
};

class IRBuilder;

static void FatalError(const char *fn, unsigned int line, const char *format, ...){
    // for product version
    //printf("Error\n");
    printf("Error\n source %s line%u : ", fn,line);
    va_list arg;
    
    va_start(arg, format);
    ::vprintf(format, arg);
    va_end(arg);
    printf("\n");
    exit(0);

}

static void Log(const char *fn, unsigned int line, const char *fmt, ...)
{
    if (!DEBUGOPTION) {
        return;
    }
    printf("Debug\n source %s line%u : ", fn, line);
    va_list arg;
    
    va_start(arg, fmt);
    ::vprintf(fmt, arg);
    va_end(arg);
    printf("\n");
}

}

#pragma GCC visibility pop
#endif
