//
//  YANodes.cpp
//  scheat
//
//  Created by かずちか on 2020/09/26.
//

#include "YANodes.h"
#include "ScheatParser.h"
using namespace scheat;
using namespace scheat::yaNodes;
using namespace std;
using namespace basics;
using scheat::statics::contextCenter;
using scheat::statics::global_context;
using scheat::statics::main_Context;
using scheat::statics::local_context;
using scheat::statics::objects;
using scheat::statics::fname;
using scheat::statics::mTokens;
using scheat::statics::scheato;

static string strreplace(string base, string target, string into){
    if (!target.empty()) {
        std::string::size_type pos = 0;
        while ((pos = base.find(target, pos)) != std::string::npos) {
            base.replace(pos, target.length(), into);
            pos += into.length();
        }
    }
    return base;
}

Value *StringTerm::codegen(IRStream &f){
    
    string substr = value;
    
    strreplace(substr, "\\0", "\\00");
    strreplace(substr, "\\n", "\\0A");
    strreplace(substr, "\n", "\\0A");
    strreplace(substr, "\t", "\\09");
    strreplace(substr, "\\t", "\\09");
    strreplace(substr, "\\\"", "\\22");
    string r = "@";
    if (global_context->strmap.find(substr) == global_context->strmap.end()) {
        r = r + to_string(global_context->strmap.size());
    }else{
        r = r + to_string(global_context->strmap[substr]);
    }
    auto rn = r;
    int strlength = substr.size() - 2;
    auto sname =  rn + ".str";
    global_context->stream_entry <<
    sname << " = private unnamed_addr constant [" <<
    to_string(strlength) << " x i8] c" << value << "\n";
    string v = "getelementptr inbounds ([" + to_string(strlength) + " x i8], [" + to_string(strlength) + " x i8]* " + sname + ", i32 0, i32 0)";
    return new Value(v, this->type);
}

unique_ptr<Term> Term::init(unique_ptr<TermNode> up){
    auto u = make_unique<Term>();
    u->lhs = move(up);
    u->type = u->lhs->type;
    return u;
}

Value *BoolTerm::codegen(IRStream &f){
    if (value) {
        return new Value(to_string(1),type);
    }else{
        return new Value(to_string(0),type);
    }
    return nullptr;
}

Value *FloatTerm::codegen(IRStream &f){
    
    return new Value(to_string(value), type);
}

Value *IntTerm::codegen(IRStream &f){
    return new Value(to_string(value), type);
}

Value *IdentifierTerm::codegen(IRStream &f){
    if (isFunc) {
        string mangled = type.ir_used + "_";
        string mtype = type.name + " (";
        string mtype_ir = type.ir_used + " (";
        mangled += value;
        for (auto v = args.begin();
             v != args.end();
             mtype += ", ",
             mtype_ir += ", ",
             v = next(v)) {
            
            mangled += "_" + (*v)->type.ir_used;
            mtype += (*v)->type.name;
            mtype_ir += (*v)->type.ir_used;
        }
        mtype += ")";
        mtype_ir += ")";
        return new Value(mangled, TypeData(mtype, mtype_ir));
    }
    return new Value(value, type);
}

void IdentifierTerm::addArg(unique_ptr<Expr> value){
    args.push_back(move(value));
}

string IdentifierTerm::userdump(){
    string ss = value;
    for (auto v = args.begin(); v != args.end(); v = next(v)) {
        ss = ss + (*v)->userdump();
    }
    return ss;
}
