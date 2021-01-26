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
#include "ScheatObjects.h"
#include "ScheatContextStructures.h"
#include "scheatPriv.hpp"
#include "ScheatStatics.h"
#include "ScheatContext.h"
#include "Lexer.hpp"
#include "Classes.h"
#include "ScheatParser2.h"
#include "ScheatEncoder.h"
#include "Utilities.h"

using namespace scheat;

llvm::LLVMContext IRBuilderReplica::context;
llvm::IRBuilder<> IRBuilderReplica::builder(IRBuilderReplica::context);
std::unique_ptr<llvm::Module> IRBuilderReplica::module;
std::map<std::string, StructureData*> IRContext::types;
std::map<std::string, VariableData*> IRContext::ids;
_Scheat *IRBuilderReplica::host = nullptr;
bool IRBuilderReplica::ready = false;

class scheat::IRBuilder {
    
public:
    IRBuilder(scheat::_Scheat*, std::string);
};

scheat::IRBuilder::IRBuilder(_Scheat *obj, std::string modname){
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

bool Scheat::hasProbrem() const{
    return schobj->hasProbrem();
}

void OldScheat::old_Debug(const char *msg, unsigned int line){
    if (!debug) {
        return;
    }
    printf("line%u : %s\n", line, msg);
}

void _Scheat::FatalError(SourceLocation location, const char *fn, unsigned int line, const char *fmt, ...){
    hasError = true;
    FILE *fp = stdout;
    if (loggingFile != "") {
        fp = fopen(loggingFile.c_str(), "a");
        if (!fp) {
            printf("logging file couldn't be opened.");
        }
    }
    if (delegate != nullptr) {
        delegate->fatalError(this,location, targettingFile, fmt);
        return;
    }
    if (deepDebug) {
        fprintf(fp, "\033[1;31mError:\033[m(from %s, line%u)\n in file: %s\n line%u.%u : ",
               fn,
               line,
               targettingFile.c_str(),
               location.line,
               location.column);
    }else{
        fprintf(fp, "\033[1;31mError:\033[m\n in file: %s\n line%u.%u : ",
               targettingFile.c_str(),
               location.line,
               location.column);
    }
    va_list arg;
    
    va_start(arg, fmt);
    ::vfprintf(fp, fmt, arg);
    va_end(arg);
    fprintf(fp, "\n");
    fclose(fp);
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

void _Scheat::Warning(SourceLocation location, const char *fn, unsigned int line, const char *format, ...){
    if (delegate != nullptr) {
        delegate->warning(this, location, sourceFile, format);
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

void _Scheat::Log(SourceLocation location, const char *fn, unsigned int line, const char *fmt, ...){
    if (!debug) {
        return;
    }
    if (delegate != nullptr) {
        delegate->log(this, location, targettingFile, fmt);
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

_Scheat::_Scheat(){
    debug = false;
    deepDebug = false;
    hasError = false;
    targettingFile = "";
    location = SourceLocation();
    delegate = nullptr;
}

_Scheat::_Scheat(Scheat *sch){
    debug = sch->debug;
    deepDebug = sch->deepDebug;
    sch->schobj = this;
    sourceFile = sch->sourceFile;
    targettingFile = sch->targettingFile;
    outputFilePath = sch->outputFilePath;
    target = sch->target;
    datalayout = sch->datalayout;
    header_search_path = sch->header_search_path;
    library_search_path = sch->library_search_path;
    delegate = sch->delegate;
    productName = sch->productName;
    delLL = sch->delLL;
    tokens = nullptr;
}

void Scheat::ready(){
    scheato = new _Scheat(this);
    ScheatContext::Init(scheato);
    if (isMain) {
        ScheatContext::AddMain();
    }
}

void Scheat::addSome(){
    if (scheato == nullptr) {
        ready();
        return;
    }
    auto newone = new _Scheat(this);
    newone->tokens = scheato->tokens;
    scheato = newone;
    ScheatContext::Init(scheato);
    if (isMain) {
        ScheatContext::AddMain();
    }
}

Scheat::Scheat(){
    debug = false;
    deepDebug = false;
    targettingFile = "";
    delegate = nullptr;
    if (scheato != nullptr) {
        scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__, "Another Scheat is initialized. One thread can have only one Scheat.");
        exit(0);
    }
//    scheato = new _Scheat(this);
//    ScheatContext::Init(scheato);
}

void _Scheat::DevLog(SourceLocation location, const char *fn, unsigned int line, const char *fmt, ...){
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

void Scheat::setProductName(string nm){
    productName = nm;
    //schobj->productName = nm;
}

void scheat::OldScheat::printVersion(){
    
    printf("%s\n", (std::to_string(version) + "." +
            std::to_string(section) + "." +
            std::to_string(part)).c_str());
}

void ScheatLexer::lex(){
    lexer::Lexer lxr(scheato);
    lxr.lex(scheato);
    if (!scheato->tokens) {
        scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__,
                            "Failed to lex and unable to continue.");
        exit(0);
    }
}

void ScheatLexer::testlex(std::string buf){
    lexer::Lexer lxr(scheato);
    lxr.lex(buf);
    lxr.getTokens()->enumerate();
}

void Scheat::complementSettings(){
    if (sourceFile == "") {
        scheato->Warning(SourceLocation(), __FILE_NAME__, __LINE__, "to complete settings, at least sourceFile is needed.");
        return;
    }
    
    if (outputFilePath == "-") {
        outputFilePath = getFileName(sourceFile);
        delLL = true;
    }else{
        delLL = false;
    }
    
    if (productName == "") {
        productName = outputFilePath;
    }
    
    if (sourceFile.find(".scheat") != string::npos) {
        isMain = true;
    }
}

void Scheat::setDebugSetting(bool o){
    debug = o;
    if (schobj != nullptr) schobj->setDebugSetting(o);
}

void Scheat::allowDeepDebug(bool b){
    deepDebug = b;
    if (schobj != nullptr) schobj->allowDeepDebug(b);
};

void ScheatAnalyzer::parse(){
    if (scheato->deepDebug) {
        scheato->tokens->enumerate();
    }
    parser2::parse(scheato, scheato->tokens);
}

void ScheatEncoder::encode(){
    ofstream fp(scheato->outputFilePath + ".ll");
    if (!fp.is_open()) {
        scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__, "%s could not  be opened.", scheato->outputFilePath.c_str());
        return;
    }
    ScheatContext::exportTo(fp);
    encoder::LLSCEncoder::encode(scheato->outputFilePath);
}

void ScheatEncoder::printout(){
    ScheatContext::printout();
}
