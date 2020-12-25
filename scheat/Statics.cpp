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
#include "ScheatNodes.h"
#include "ScheatContext.h"
#include "Classes.h"
#include "ScheatStatics.h"

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
    contextCenter.push_back(ScheatContext::global);
    global->stream_entry << "source_filename = \"" << scheato->sourceFile << "\"\n";
    global->stream_entry << "target datalayout = \"" << scheato->datalayout << "\"\n";
    global->stream_entry << "target triple = \"" << scheato->target << "\"\n\n";
    global->stream_body << "%ScheatShortConstString = type{ i8, [7 x i8] }\n";
    global->stream_body << "%Array = type{ i32, i64, i8* }\n";
    global->stream_body << "%union_buf = type{ %ScheatShortConstString }\n";
    global->stream_body << "%String = type{ %union_buf }\n";
    global->stream_body << "declare void @print_return()\n";
    global->stream_body << "declare void @print_i32(i32)\n";
    global->stream_body << "declare void @print_i8x(i8*)\n";
    global->stream_body << "declare void @printn()\n";
    global->stream_body << "declare void @print_i1(i1)\n";
    global->stream_body << "declare i8* @ScheatPointer_alloc(i64, void(i8*)*)\n";
    global->stream_body << "declare void @ScheatPointer_copy(i8*)\n";
    global->stream_body << "declare void @ScheatPointer_unref(i8*)\n";
    global->stream_body << "declare void @ScheatPointer_release(i8*)\n";
    global->stream_body << "declare void @print_String(%String*)\n";
    global->stream_body << "declare %String @String_init(i8*)\n";
    global->stream_body << "declare %String* @String_add(%String*, %String*)\n";
    global->stream_body << "declare %String* @String_copy(%String*)\n";
    global->stream_body << "declare void @Array_append(%Array*, i8*)\n";
    global->stream_body << "declare i8* @Array_at(%Array*, i32)\n";
    global->stream_body << "declare %Array @Array_init(i64)\n";
    Function *initf = new Function("void", "main");
    initf->context->stream_entry << "define void @" << scheato->productName << "_init(){\n";
    initf->context->stream_entry << "entry:\n";
    initf->context->stream_tail << "ret void\n}\n";
    contextCenter.push_back(initf->context);
    init = initf->context;
    pushNewNamespace(scheato->productName);
    ScheatContext::global->addFunction(scheato->productName + "_init", initf);
    ScheatContext::main = nullptr;
    mTokens = nullptr;
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
    
    ScheatContext::global->addClass("Int", Int);
    auto String = new Class(new TypeData("String"));
    ScheatContext::global->addClass("String", String);
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
    ScheatContext::main->stream_entry << "call void() @" << scheato->sourceFile + "_init()\n";
    ScheatContext::main->stream_body << "%argc = alloca i32\n";
    ScheatContext::main->stream_body << "%argv = alloca i8**\n";
    ScheatContext::main->stream_body << "store i32 %0, i32* %argc\n";
    ScheatContext::main->stream_body << "store i8** %1, i8*** %argv\n";
    ScheatContext::main->stream_tail << "ret i32 0\n}\n";
}

stack<Context *> ScheatContext::localcon;
Context *ScheatContext::global = nullptr;
vector<Context *> ScheatContext::contextCenter = {};
stack<string> ScheatContext::namespace_center;
Context *ScheatContext::init = nullptr;


}
