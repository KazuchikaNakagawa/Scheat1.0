//
//  Statics.cpp
//  scheat
//
//  Created by かずちか on 2020/09/13.
//

#include <stdio.h>
#include <vector>
#include <stack>
#include <map>
#include <string>
//#include "ScheatNodes.h"
#include "ScheatContext.h"
#include "Classes.h"
#include "ScheatStatics.h"
#include "Utilities.h"
#include <llvm/Support/Host.h>
#include <llvm/IR/Module.h>
#include <llvm/Linker/Linker.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/LinkAllIR.h>
#include <system_error>

namespace scheat {

using namespace scheat;

//std::vector<Context *> ScheatContext::contextCenter = {};
//Context *ScheatContext::global = nullptr;
Context *ScheatContext::main = nullptr;
//std::stack<Context *> ScheatContextData::local() = {};
std::map<int, std::vector<std::string>> objects = {};
std::string fname = "";
Token *mTokens = nullptr;
_Scheat *scheato = nullptr;
IRStream initStream;

void ScheatContext::Init(_Scheat *sch){
    scheato = sch;
    global = new Context();
    global->name = "global";
    localcon.push(ScheatContext::global);
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    std::string Error;
    auto triple = llvm::sys::getDefaultTargetTriple();
    auto Target = llvm::TargetRegistry::lookupTarget(triple, Error);
    
    sch->datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128";
    if (!Target) {
        llvm::errs() << Error;
        scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__,
                            "target is unclear.");
        
    }else{
        
    }
    contextCenter.push_back(ScheatContext::global);
    global->stream_entry << "; ModuleID = '" << scheato->sourceFile << "'\n";
    global->stream_entry << "source_filename = \"" << scheato->sourceFile << "\"\n";
    global->stream_entry << "target datalayout = \"" << scheato->datalayout << "\"\n";
    global->stream_entry << "target triple = \"" << triple << "\"\n\n";
    global->stream_body << "%ScheatShortConstString = type{ i8, [7 x i8] }\n";
    global->stream_body << "%Array = type{ i32, i64, i8* }\n";
    global->stream_body << "%union_buf = type{ i8* }\n";
    global->stream_body << "%String = type{ %union_buf }\n";
    global->stream_body << "declare void @print_return()\n";
    global->stream_body << "declare void @print_i32(i32)\n";
    global->stream_body << "declare void @print_i8x(i8*)\n";
    //global->stream_body << "declare void @_print_return()\n";
    global->stream_body << "declare void @print_i1(i1)\n";
    global->stream_body << "declare i8* @ScheatPointer_alloc(i64, void(i8*)*)\n";
    global->stream_body << "declare void @ScheatPointer_copy(i8*)\n";
    global->stream_body << "declare void @ScheatPointer_unref(i8*)\n";
    global->stream_body << "declare void @ScheatPointer_release(i8*)\n";
    global->stream_body << "declare void @print_String(%String)\n";
    global->stream_body << "declare %String @String_init_pi8(i8*)\n";
    global->stream_body << "declare %String @String_init()\n";
    global->stream_body << "declare %String @String_add(%String, %String)\n";
    global->stream_body << "declare %String @String_copy(%String)\n";
    global->stream_body << "declare i32 @String_count(%String*)\n";
    global->stream_body << "declare void @Array_append(%Array*, i8*)\n";
    global->stream_body << "declare i8* @Array_at(%Array*, i32)\n";
    global->stream_body << "declare %Array @Array_init(i64)\n";
    ScheatContext::global->stream_body << "declare %String @inputString()\n";
    ScheatContext::global->stream_body << "declare i32 @inputInt()\n";
    Function *initf = new Function("void", "init");
    initf->context->stream_entry << "define void @" << getFileName(scheato->sourceFile) << "_init(){\n";
    initf->context->stream_entry << "entry:\n";
    initf->context->stream_tail << "ret void\n}\n";
    push(initf->context);
    contextCenter.push_back(initf->context);
    init = initf->context;
    pushNewNamespace(scheato->productName);
    ScheatContext::global->addFunction(getFileName(scheato->sourceFile) + "_init", initf);
    ScheatContext::main = nullptr;
    mTokens = nullptr;
    Function *inputf_int = new Function("i32", "inputInt");
    inputf_int->return_type = TypeData::IntType;
    ScheatContext::global->addFunction("inputInt_", inputf_int);
    Function *inputf_str = new Function("%String", "inputString");
    inputf_str->return_type = TypeData::StringType;
    ScheatContext::global->addFunction("inputString_", inputf_str);
    auto Int = new Class(new TypeData("i32"));
    Int->context->name = "Int";
    auto opadd = new Operator("+", "add");
    opadd->position = infix;
    opadd->precidence = secondary;
    opadd->return_type = TypeData("Int", "i32");
    opadd->lhs_type = &TypeData::IntType;
    opadd->rhs_type = &TypeData::IntType;
    Int->operators["+"] = opadd;
    
    auto opsub = new Operator("-", "sub");
    opsub->position = infix;
    opsub->precidence = secondary;
    opsub->return_type = TypeData("Int", "i32");
    opsub->lhs_type = &TypeData::IntType;
    opsub->rhs_type = &TypeData::IntType;
    Int->operators["-"] = opsub;
    
    auto opmul = new Operator("*", "mul");
    opmul->position = infix;
    opmul->precidence = primary;
    opmul->return_type = TypeData("Int", "i32");
    opmul->lhs_type = &TypeData::IntType;
    opmul->rhs_type = &TypeData::IntType;
    Int->operators["*"] = opmul;
    
    auto opeq = new Operator("==", "eqeq");
    opeq->position = infix;
    opeq->precidence = secondary;
    opeq->return_type = TypeData::BoolType;
    opeq->lhs_type = &TypeData::IntType;
    opeq->rhs_type = &TypeData::IntType;
    Int->operators["=="] = opeq;
    
    ScheatContext::global->addClass("Int", Int);
    auto String = new Class(new TypeData("String", "%String"));
    auto countFunction = new ExternalFunction(TypeData::IntType, "String_count");
    countFunction->return_type = TypeData::IntType;
    countFunction->argTypes.push_back(TypeData::StringType.pointer());
    String->context->addFunction("count", countFunction);
    
    ScheatContext::global->addClass("String", String);
    
    auto Bool = new Class(&TypeData::BoolType);
    Bool->context->name = "Bool";
    
}

Context *ScheatContext::local(){
    
    return localcon.top();
}

void ScheatContext::printout(){
    for (auto c : contextCenter) {
        c->stream_entry.printout();
        c->stream_body.printout();
        c->stream_tail.printout();
    }
}

void ScheatContext::AddMain(){
    Function *mainf = new Function("i32", "main");
    mainf->argTypes.push_back(TypeData("i32"));
    mainf->argTypes.push_back(TypeData("i8**"));
    mainf->return_type = TypeData("i32");
    ScheatContext::main = mainf->context;
    ScheatContext::contextCenter.push_back(ScheatContext::main);
    Variable *argc = new Variable("argc", TypeData("i32"));
    ScheatContext::main->addVariable("argc", argc);
    Variable *argv = new Variable("argv", TypeData("i8**"));
    ScheatContext::main->addVariable("argv", argv);
    mainf->codegen(ScheatContext::main->stream_entry);
    ScheatContext::push(ScheatContext::main);
    ScheatContext::main->stream_entry << "entry:\n";
    ScheatContext::main->stream_entry << "call void @" << getFileName(scheato->sourceFile) + "_init()\n";
//    auto nargv = new Variable("argv", TypeData("Array of String", "%Array"));
//    ScheatContext::main->addVariable("argv", nargv);
//    ScheatContext::main->stream_body << "%argv = alloca %Array\n";
//    ScheatContext::main->stream_body << "%far = call %Array(i64) @Array_init(i64 8)\n";
//    ScheatContext::main->stream_body << "store %Array %far, %Array* %argv\n";
//    ScheatContext::main->stream_body << "%counter = i32 0\n";
//    ScheatContext::main->stream_body << "ready_condition:\n";
//    ScheatContext::main->stream_body << "%limit = sub nsw i32 %0, 1\n";
//    ScheatContext::main->stream_body << "%cond = icmp ne i32 %limit, %counter\n";
//    ScheatContext::main->stream_body << "br i1 %cond, label %ready_body, label %ready_end\n";
//    ScheatContext::main->stream_body << "ready_body:\n";
//    ScheatContext::main->stream_body << "%str_element = i8* getelementptr inbounds i8** %1, i32 0, i32 %counter\n";
//    ScheatContext::main->stream_body << "call void(%Array*, i8*) @Array_append(%Array* %argv, i8* %str_element)\n";
//    ScheatContext::main->stream_body << "%str_element = add nsw i32 %counter, 1\n";
//    ScheatContext::main->stream_body << "br label %ready_condition\n";
//    ScheatContext::main->stream_body << "ready_end:\n";
    //ScheatContext::main->stream_body << "%argc = alloca i32\n";
    //ScheatContext::main->stream_body << "%argv = alloca i8**\n";
    //ScheatContext::main->stream_body << "store i32 %0, i32* %argc\n";
    //ScheatContext::main->stream_body << "store i8** %1, i8*** %argv\n";
    ScheatContext::main->stream_tail << "ret i32 0\n}\n";
    //ScheatContext::main->stream_body << "call void @" << scheato->sourceFile + "_init()\n";
}

stack<Context *> ScheatContext::localcon;
Context *ScheatContext::global = nullptr;
vector<Context *> ScheatContext::contextCenter = {};
stack<string> ScheatContext::namespace_center;
Context *ScheatContext::init = nullptr;


}
