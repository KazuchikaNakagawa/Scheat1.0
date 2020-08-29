//
//  scheat.cpp
//  scheat
//
//  Created by かずちか on 2020/07/30.
//

#include <iostream>
#include <stdio.h>
#include <cstdarg>
#include "scheat.h"
#include "scheatPriv.hpp"

using namespace scheat;

llvm::LLVMContext IRBuilderReplica::context;
llvm::IRBuilder<> IRBuilderReplica::builder(IRBuilderReplica::context);
std::unique_ptr<llvm::Module> IRBuilderReplica::module;
std::map<std::string, StructureData*> IRContext::types;
std::map<std::string, VariableData*> IRContext::ids;
Scheat *IRBuilderReplica::host = nullptr;
bool IRBuilderReplica::ready = false;

class scheat::IRBuilder {
    
public:
    IRBuilder(scheat::Scheat*, std::string);
};

scheat::IRBuilder::IRBuilder(Scheat *obj, std::string modname){
    if (IRBuilderReplica::isReady()) {
        return;
    }
    IRBuilderReplica::host = obj;
    IRBuilderReplica::module = std::make_unique<llvm::Module>(modname, IRBuilderReplica::context);
    IRBuilderReplica::setReady();
}

void IRBuilderReplica::check(){
    if (!ready) {
        if (!ready) {
            printf("ERROR: TO DEVELOPER\n");
            printf("    YOU NEED TO CALL IRBuilder::IRBuilder() FIRST.\n");
            exit(0);
        }
        exit(0);
    }
}

void OldScheat::old_Debug(const char *msg, unsigned int line){
    if (!debug) {
        return;
    }
    printf("line%u : %s\n", line, msg);
}

void Scheat::FatalError(const char *fn, unsigned int line, const char *fmt, ...){
    hasError = true;
    if (delegate != nullptr) {
        delegate->fatalError(location, targettingFile, fmt);
        return;
    }
    if (deepDebug) {
        printf("\033[1;31mError:\033[m(from %s, line%u)\n in file: %s\n line%u.%u : ",
               fn,
               line,
               targettingFile.c_str(),
               location.line,
               location.column);
    }else{
        printf("\033[1;31mError:\033[m\n in file: %s\n line%u.%u : ",
               targettingFile.c_str(),
               location.line,
               location.column);
    }
    va_list arg;
    
    va_start(arg, fmt);
    ::vprintf(fmt, arg);
    va_end(arg);
    printf("\n");
    exit(0);
}

void OldScheat::Log(const char *fn, unsigned int line, const char *fmt, ...)
{
    if (!debug) {
        return;
    }
    printf("\033[1mDebug\033[m\n in file %s\n line%u : ", fn, line);
    va_list arg;
    
    va_start(arg, fmt);
    ::vprintf(fmt, arg);
    va_end(arg);
    printf("\n");
}

void OldScheat::old_FatalError(const char *msg, unsigned int line){
    printf("FatalError %u : %s\n", line, msg);
    exit(0);
}

void Scheat::Warning(const char *fn, unsigned int line, const char *format, ...){
    if (delegate != nullptr) {
        return;
    }
    if (deepDebug) {
        printf("\033[1;43mWarning:\033[m(from %s, line%u)\n in file: %s\n line%u.%u : ",
               fn,
               line,
               targettingFile.c_str(),
               location.line,
               location.column);
    }else{
        printf("\033[1;43mWarning:\033[m\n in file: %s\n line%u.%u : ",
               targettingFile.c_str(),
               location.line,
               location.column);
    }
    va_list arg;
    
    va_start(arg, format);
    ::vprintf(format, arg);
    va_end(arg);
    printf("\n");
}

void Scheat::Log(const char *fn, unsigned int line, const char *fmt, ...){
    if (!debug) {
        return;
    }
    if (delegate != nullptr) {
        delegate->log(location, targettingFile, fmt);
        return;
    }
    if (deepDebug) {
        printf("\033[1mLog:\033[m(from %s, line%u)\n in file: %s\n line%u.%u : ",
               fn,
               line,
               targettingFile.c_str(),
               location.line,
               location.column);
    }else{
        printf("\033[1mLog:\033[m\n in file: %s\n line%u.%u : ",
               targettingFile.c_str(),
               location.line,
               location.column);
    }
    va_list arg;
    
    va_start(arg, fmt);
    ::vprintf(fmt, arg);
    va_end(arg);
    printf("\n");
}

Scheat::Scheat(){
    debug = false;
    deepDebug = false;
    hasError = false;
    targettingFile = "";
    location = SourceLocation();
    delegate = nullptr;
}

void Scheat::DevLog(const char *fn, unsigned int line, const char *fmt, ...){
    if (deepDebug) {
        printf("\033[1mLog:\033[m(from %s, line%u)\n in file: %s\n line%u.%u : ",
               fn,
               line,
               targettingFile.c_str(),
               location.line,
               location.column);
        va_list arg;
        
        va_start(arg, fmt);
        ::vprintf(fmt, arg);
        va_end(arg);
        printf("\n");
        
    }else{
        return;
    }
    
}

void scheatPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

class Statement {
    
public:
    
};

class DUMSCHEAT {
    int version;
    
    int section;
    
    int part;
    
    const char *target;
    
    bool debug;
public:
    
};

scheat::OldScheat::OldScheat(const char *format, const char *target, bool debugOpt){
    version = 0; section = 0; part = 0;
    if (sscanf(format,
               "%d.%d.%d",
               &version,
               &section,
               &part
               ) != 3) {
        Log(__FILE_NAME__,__LINE__, "Warning: Illegal version");
        
    }
}

void scheat::OldScheat::printVersion(){
    
    printf("%s\n", (std::to_string(version) + "." +
            std::to_string(section) + "." +
            std::to_string(part)).c_str());
}

