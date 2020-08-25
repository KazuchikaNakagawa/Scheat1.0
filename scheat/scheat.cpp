//
//  scheat.cpp
//  scheat
//
//  Created by かずちか on 2020/07/30.
//

#include <iostream>
#include <stdio.h>
#include <cstdarg>
#include "scheat.hpp"
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

void Scheat::HelloWorld(const char * s)
{
    scheatPriv *theObj = new scheatPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

Scheat::Scheat(int version, int section, int part, const char *target, bool debugOpt){
    this->version = version;
    this->section = section;
    this->part = part;
    this->target = target;
    this->debug = debugOpt;
    DEBUGOPTION = debugOpt;
}

void Scheat::old_Debug(const char *msg, unsigned int line){
    if (!debug) {
        return;
    }
    printf("line%u : %s\n", line, msg);
}

void Scheat::FatalError(const char *fn, unsigned int line, const char *fmt, ...){

    printf("Error\n source %s line%u : ", fn, line);
    va_list arg;
    
    va_start(arg, fmt);
    ::vprintf(fmt, arg);
    va_end(arg);
    printf("\n");
    exit(0);
}

void Scheat::Log(const char *fn, unsigned int line, const char *fmt, ...)
{
    if (!debug) {
        return;
    }
    printf("Debug\n source %s line%u : ", fn, line);
    va_list arg;
    
    va_start(arg, fmt);
    ::vprintf(fmt, arg);
    va_end(arg);
    printf("\n");
}

void Scheat::old_FatalError(const char *msg, unsigned int line){
    printf("FatalError %u : %s\n", line, msg);
    exit(0);
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

scheat::Scheat::Scheat(const char *format, const char *target, bool debugOpt){
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

void scheat::Scheat::printVersion(){
    
    printf("%s\n", (std::to_string(version) + "." +
            std::to_string(section) + "." +
            std::to_string(part)).c_str());
}

