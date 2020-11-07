//
//  Initializer.cpp
//  scheat
//
//  Created by かずちか on 2020/11/07.
//

#include <stdio.h>
#include "Initializers.h"
#include "ScheatBasicStructures.h"
using scheat::statics::contextCenter;
using scheat::statics::global_context;
using scheat::statics::main_Context;
using scheat::statics::local_context;
using scheat::statics::objects;
using scheat::statics::fname;
using scheat::statics::mTokens;
using scheat::statics::scheato;

void scheat::InitializeAll(){
    InitializeGlobals();
    
}

void scheat::InitializeGlobals(){
    global_context = new Context();
    global_context->name = "Global";
    local_context.push(global_context);
    contextCenter.push_back(global_context);
    global_context->stream_entry << "source_filename = \"" << scheato->sourceFile << "\"\n";
    global_context->stream_entry << "target datalayout = \"" << scheato->datalayout << "\"\n";
    global_context->stream_entry << "target triple = \"" << scheato->target << "\"\n\n";
    global_context->stream_body << "%ScheatShortConstString = type{ i8, [7 x i8] }\n";
    global_context->stream_body << "%Array = type{ i32, i64, i8* }\n";
    global_context->stream_body << "%union_buf = type{ %ScheatShortConstString }\n";
    global_context->stream_body << "%String = type{ %union_buf }\n";
    global_context->stream_body << "declare void @print_return()\n";
    global_context->stream_body << "declare void @print_i32(i32)\n";
    global_context->stream_body << "declare void @print_i8x(i8*)\n";
    global_context->stream_body << "declare void @printn()\n";
    global_context->stream_body << "declare void @print_i1(i1)\n";
    global_context->stream_body << "declare i8* @ScheatPointer_alloc(i64, void(i8*)*)\n";
    global_context->stream_body << "declare void @ScheatPointer_copy(i8*)\n";
    global_context->stream_body << "declare void @ScheatPointer_unref(i8*)\n";
    global_context->stream_body << "declare void @ScheatPointer_release(i8*)\n";
    global_context->stream_body << "declare void @print_String(%String*)\n";
    global_context->stream_body << "declare %String @String_init(i8*)\n";
    global_context->stream_body << "declare %String* @String_add(%String*, %String*)\n";
    global_context->stream_body << "declare %String* @String_copy(%String*)\n";
    global_context->stream_body << "declare void @Array_append(%Array*, i8*)\n";
    global_context->stream_body << "declare i8* @Array_at(%Array*, i32)\n";
    global_context->stream_body << "declare %Array @Array_init(i64)\n";
    //global_context->stream_body << "declare i8* ";
    main_Context = nullptr;
    mTokens = nullptr;
}

void scheat::InitializeMain(){
    Function *mainf = new Function("i32", "main");
    mainf->argTypes.push_back(TypeData("i32"));
    mainf->argTypes.push_back(TypeData("i8**"));
    mainf->return_type = TypeData("i32");
    main_Context = mainf->context;
    contextCenter.push_back(main_Context);
    Variable *argc = new Variable("argc", TypeData("i32"));
    main_Context->addVariable("argc", argc);
    Variable *argv = new Variable("argv", TypeData("i8**"));
    main_Context->addVariable("argv", argv);
    mainf->codegen(main_Context->stream_entry);
    main_Context->stream_body << "entry:\n";
    main_Context->stream_body << "%argc = alloca i32\n";
    main_Context->stream_body << "%argv = alloca i8**\n";
    main_Context->stream_body << "store i32 %0, i32* %argc\n";
    main_Context->stream_body << "store i8** %1, i8*** %argv\n";
    main_Context->stream_tail << "ret i32 0\n}\n";
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
    
    global_context->addClass("Int", Int);
    auto String = new Class(new TypeData("String"));
    global_context->addClass("String", String);
}
