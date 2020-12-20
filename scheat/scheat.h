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

#include "Classes.h"
namespace scheat{

struct SourceLocation;

class _Scheat;

class ScheatDelegate {
public:
    
    virtual void fatalError(_Scheat *_scheat,SourceLocation location, std::string filePath, std::string message, ...);
    
    virtual void warning(_Scheat *_scheat, SourceLocation location, std::string filePath, std::string message, ...);
    
    virtual void log(_Scheat *_scheat, SourceLocation location, std::string filePath, std::string message, ...);
    
    virtual std::string target_triple();
    
    virtual std::string datalayout();
    
};

class _Scheat;

class Scheat {
    bool debug;
    bool deepDebug;
    _Scheat *schobj = nullptr;
    std::string productName;
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
    void setDebugSetting(bool o) ;
    void allowDeepDebug(bool b);
    void setProductName(std::string);
    void ready();
    Scheat();
    friend class _Scheat;
};;

class IRBuilder;

}

#pragma GCC visibility pop
#endif
