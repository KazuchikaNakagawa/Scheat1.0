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
#include "ScheatStd.hpp"
#include "Classes.h"
namespace scheat{

using namespace scheatSTD;

class _Scheat;
using namespace std;
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
    bool isMain = false;
    std::string sourceFile = "";
    std::string targettingFile;
    std::string outputFilePath;
    std::string target;
    std::string datalayout;
    vector<std::string> header_search_path;
    vector<std::string> library_search_path;
    vector<string> linkPaths = {};
    ScheatDelegate *delegate = nullptr;
    bool deletesIRFiles = true;
    bool onlyAssemble = false;
    bool logTopString = false;
    void complementSettings();
    void logInfo(bool o) ;
    void logAllInfo(bool b);
    void setProductName(std::string);
    void ready();
    void addMore();
    Scheat();
    friend class _Scheat;
};;

class IRBuilder;

}

#pragma GCC visibility pop
#endif
