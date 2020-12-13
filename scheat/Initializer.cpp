//
//  Initializer.cpp
//  scheat
//
//  Created by かずちか on 2020/11/07.
//

#include <stdio.h>
#include "Initializers.h"
#include "ScheatTypes.h"
using namespace scheat::statics;
using scheat::statics::objects;
using scheat::statics::fname;
using scheat::statics::mTokens;
using scheat::statics::scheato;

void scheat::InitializeAll(){
    InitializeGlobals();
    
}

void scheat::InitializeGlobals(){
    ScheatContext::global = new Context();
    ScheatContext::global->name = "Global";
    ScheatContext::push(ScheatContext::global);
    ScheatContext::contextCenter.push_back(ScheatContext::global);
    ScheatContext::global->stream_entry << "source_filename = \"" << scheato->sourceFile << "\"\n";
    ScheatContext::global->stream_entry << "target datalayout = \"" << scheato->datalayout << "\"\n";
    ScheatContext::global->stream_entry << "target triple = \"" << scheato->target << "\"\n\n";
    ScheatContext::global->stream_body << "%ScheatShortConstString = type{ i8, [7 x i8] }\n";
    ScheatContext::global->stream_body << "%Array = type{ i32, i64, i8* }\n";
    ScheatContext::global->stream_body << "%union_buf = type{ %ScheatShortConstString }\n";
    ScheatContext::global->stream_body << "%String = type{ %union_buf }\n";
    ScheatContext::global->stream_body << "declare void @print_return()\n";
    ScheatContext::global->stream_body << "declare void @print_i32(i32)\n";
    ScheatContext::global->stream_body << "declare void @print_i8x(i8*)\n";
    ScheatContext::global->stream_body << "declare void @printn()\n";
    ScheatContext::global->stream_body << "declare void @print_i1(i1)\n";
    ScheatContext::global->stream_body << "declare i8* @ScheatPointer_alloc(i64, void(i8*)*)\n";
    ScheatContext::global->stream_body << "declare void @ScheatPointer_copy(i8*)\n";
    ScheatContext::global->stream_body << "declare void @ScheatPointer_unref(i8*)\n";
    ScheatContext::global->stream_body << "declare void @ScheatPointer_release(i8*)\n";
    ScheatContext::global->stream_body << "declare void @print_String(%String*)\n";
    ScheatContext::global->stream_body << "declare %String @String_init(i8*)\n";
    ScheatContext::global->stream_body << "declare %String* @String_add(%String*, %String*)\n";
    ScheatContext::global->stream_body << "declare %String* @String_copy(%String*)\n";
    ScheatContext::global->stream_body << "declare void @Array_append(%Array*, i8*)\n";
    ScheatContext::global->stream_body << "declare i8* @Array_at(%Array*, i32)\n";
    ScheatContext::global->stream_body << "declare %Array @Array_init(i64)\n";
    Function *initf = new Function("void", "main");
    initf->context->stream_entry << "define void @" << scheato->sourceFile << "_init(){\n";
    initf->context->stream_tail << "ret void\n}\n";
    ScheatContext::contextCenter.push_back(initf->context);
    ScheatContext::main = nullptr;
    mTokens = nullptr;
}

void scheat::InitializeMain(){
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
    ScheatContext::main->stream_body << "entry:\n";
    ScheatContext::main->stream_body << "%argc = alloca i32\n";
    ScheatContext::main->stream_body << "%argv = alloca i8**\n";
    ScheatContext::main->stream_body << "store i32 %0, i32* %argc\n";
    ScheatContext::main->stream_body << "store i8** %1, i8*** %argv\n";
    ScheatContext::main->stream_tail << "ret i32 0\n}\n";
}

static scheat::TypeData IntType = scheat::TypeData("Int", "i32");
static scheat::TypeData StringType = scheat::TypeData("String", "%String");

void scheat::InitializeFoundationClass(){
    auto Int = new Class(new TypeData("i32"));
    Int->context->name = "Int";
    auto opadd = new Operator("+", "add");
    opadd->position = basics::infix;
    opadd->precidence = basics::secondary;
    opadd->return_type = TypeData("Int", "i32");
    opadd->lhs_type = &IntType;
    opadd->rhs_type = &IntType;
    Int->operators["+"] = opadd;
    
    auto opsub = new Operator("-", "sub");
    opsub->position = basics::infix;
    opsub->precidence = basics::secondary;
    opsub->return_type = TypeData("Int", "i32");
    opsub->lhs_type = &IntType;
    opsub->rhs_type = &IntType;
    Int->operators["-"] = opsub;
    
    auto opmul = new Operator("*", "mul");
    opmul->position = basics::infix;
    opmul->precidence = basics::primary;
    opmul->return_type = TypeData("Int", "i32");
    opmul->lhs_type = &IntType;
    opmul->rhs_type = &IntType;
    Int->operators["*"] = opmul;
    
    ScheatContext::global->addClass("Int", Int);
    auto String = new Class(new TypeData("String"));
    ScheatContext::global->addClass("String", String);
}
