//
//  YANodes.cpp
//  scheat
//
//  Created by かずちか on 2020/09/26.
//

#include "Nodes2.h"
#include "ScheatStatics.h"
#include <regex>
//#include <dlfcn.h>
using namespace scheat;
using namespace scheat::nodes2;
using namespace std;


static string strreplace(string &str, string target, string into){
    string base = str;
    if (target == "") {
        return base;
    }
    string copy = base;
    if (base == "") {
        return base;
    }
    string result;
    vector<unsigned int> lists = {};
    while (true) {
        auto index = base.find(target);
        if (index == string::npos) {
            break;
        }
        base = base.substr(0, index);
        lists.push_back(index);
    }
    if (lists.empty()) {
        return result;
    }
    for (int i = 0; i < copy.size(); i++) {
        if (lists.empty()) {
            result.push_back(copy[i]);
            continue;
        }
        if (i == lists[0]) {
            result = result + into;
            i += target.size() - 1;
            lists.erase(lists.begin());
        }else{
            result.push_back(copy[i]);
        }
    }
    str = result;
    return result;
}

static TypeData asPointer(TypeData ty){
    return TypeData("the " + ty.name, ty.ir_used + "*");
}


string InfixOperatorTerm::userdump(){
    return lhs->userdump() + op->value + rhs->userdump();
}

string PostfixOperatorTerm::userdump(){
    return lhs->userdump() + op->value;
}

string PrefixOperatorTerm::userdump(){
    return op->value + rhs->userdump();
}

string InfixOperatorExpr::userdump(){
    return rhs->userdump() + op->value + lhs->userdump();
}

string PrefixOperatorExpr::userdump(){
    return op->value + rhs->userdump();
}

string PostfixOperatorExpr::userdump(){
    return lhs->userdump() + op->value;
}

string VariableTerm::userdump(){
    return value;
}

string FunctionCallTerm::userdump(){
    return func->getMangledName();
}

unique_ptr<FunctionCallTerm> FunctionCallTerm::init(Token *token, Function *func){
    auto ptr = make_unique<FunctionCallTerm>();
    ptr->func = func;
    ptr->value = token->value.strValue;
    ptr->args.clear();
    ptr->value = "FUNCTION";
    return ptr;
}

Value *VariableAttributeExpr::codegenAsRef(IRStream &f){
    
    return nullptr;
}

Value *GlobalExpr::codegenAsRef(IRStream &f){
    auto v = ptr->codegen(f);
    return v;
}

Value *GlobalExpr::codegen(IRStream &f){
    auto v = codegenAsRef(f);
    string reg = ScheatContext::local()->getRegister();
    f << reg << " = load " << v->type.ir_used << ", " << v->asValue() << "\n";
    return new Value(reg, v->type);
}

unique_ptr<GlobalExpr> GlobalExpr::init(unique_ptr<TopIdentifierExpr> ptr){
    auto p = make_unique<GlobalExpr>();
    p->location = ptr->location;
    p->type = ptr->type;
    p->ptr = move(ptr);
    return p;
}

Value *PostfixOperatorExpr::codegen(IRStream &f){
    auto l = lhs->codegen(f);
    if (op->return_type.name == "Void") {
        f << "call void(" << op->lhs_type->ir_used << ") " << op->func_name << "(" << l->asValue() << ")\n";
        delete l;
        return nullptr;
    }else{
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << op->return_type.ir_used << "(" << op->rhs_type->ir_used << ") " << op->func_name << "(" << l->asValue() << ")\n";
        delete l;
        return new Value(reg, op->return_type);
    }
    return nullptr;
}

Value *PrefixOperatorExpr::codegen(IRStream &f){
    auto r = rhs->codegen(f);
    if (op->return_type.name == "Void") {
        f << "call void(" << op->rhs_type->ir_used << ") " << op->func_name << "(" << r->asValue() << ")\n";
        delete r;
        return nullptr;
    }else{
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << op->return_type.ir_used << "(" << op->rhs_type->ir_used << ") " << op->func_name << "(" << r->asValue() << ")\n";
        delete r;
        return new Value(reg, op->return_type);
    }
    return nullptr;
}

Value *PostfixOperatorPrimaryExpr::codegen(IRStream &f){
    auto lv = lhs->codegen(f);
    if (op->return_type.name == "Void") {
        f << "call void(" << op->lhs_type->ir_used << ") " << op->func_name << "(" << lv->asValue() << ")\n";
        delete lv;
        return nullptr;
    }else{
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << op->return_type.ir_used << "(" << op->lhs_type->ir_used << ") " << op->func_name << "(" << lv->asValue() << ")\n";
        delete lv;
        return new Value(reg, op->return_type);
    }
    return nullptr;
}

Value *FunctionCallTerm::codegen(IRStream &f){
    if (func->return_type.name == "Void") {
        vector<Value *> arges_val = {};
        for (int i = 0; i < args.size(); i++) {
            arges_val.push_back(args[i]->codegen(f));
        }
        f << "call " << func->lltype() << " " << func->getMangledName() << "(";
        for (auto ptr : arges_val) {
            f << ptr->asValue();
        }
        f << ")\n";
        return nullptr;
    }else{
        vector<Value *> arges_val = {};
        for (int i = 0; i < args.size(); i++) {
            arges_val.push_back(args[i]->codegen(f));
        }
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << func->lltype() << " " << func->getMangledName() << "(";
        for (auto ptr : arges_val) {
            f << ptr->asValue();
        }
        f << ")\n";
        return new Value(reg, func->return_type);
    }
    return nullptr;
}

Value *IfStatement::codegen(IRStream &f){
    auto condv = condition->codegen(f);
    auto labels = ScheatContext::local()->getIfLabel();
    f << "br " << condv->asValue() << ", label %" << labels.first << ", label %" << labels.second << "\n";
    f << labels.first << ":\n";
    thenS->codegen(f);
    f << "br label %" << labels.first + "_end\n";
    f << labels.second << ":\n";
    elseS->codegen(f);
    f << labels.second << labels.first + "_end\n";
    f << labels.first + "_end:\n";
    return nullptr;
}

Value *InfixOperatorExpr::codegen(IRStream &f){
    auto l = lhs->codegen(f);
    auto r = rhs->codegen(f);
    if (l->type == TypeData::IntType &&
        r->type == TypeData::IntType) {
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = " << op->func_name << " nsw i32 " << l->value << ", " << r->value << "\n";
        return new Value(reg, TypeData::IntType);
    }
    if (op->return_type.name == "Void") {
        f << "call void(" << op->lhs_type->ir_used << ", " << op->rhs_type->ir_used << ") " << op->func_name << "(" << l->asValue() << ", " << r->asValue() << ")\n";
        delete l;
        delete r;
        return nullptr;
    }else{
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << op->return_type.ir_used << "(" << op->lhs_type->ir_used << ", " << op->rhs_type->ir_used << ") " << op->func_name << "(" << l->asValue() << ", " << r->asValue() << ")\n";
        delete l;
        delete r;
        return new Value(reg, op->return_type);
    }
    return nullptr;
}

Value *PostfixOperatorTerm::codegen(IRStream &f){
    auto l = lhs->codegen(f);
    if (op->return_type.name == "Void") {
        f << "call void(" << op->lhs_type->ir_used << ") " << op->func_name << "(" << l->asValue() << ")\n";
        delete l;
        return nullptr;
    }else{
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << op->return_type.ir_used << "(" << op->rhs_type->ir_used << ") " << op->func_name << "(" << l->asValue() << ")\n";
        delete l;
        return new Value(reg, op->return_type);
    }
    return nullptr;
}

Value * InfixOperatorTerm::codegen(IRStream &f){
    auto l = lhs->codegen(f);
    auto r = rhs->codegen(f);
    if (l->type == TypeData::IntType) {
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = " << op->func_name << " nsw i32 " << l->value << ", " << r->value
        << "\n";
        delete l;
        delete r;
        return new Value(reg, TypeData::IntType);
    }
    if (op->return_type.name == "Void") {
        f << "call void(" << op->lhs_type->ir_used << ", " << op->rhs_type->ir_used << ") " << op->func_name << "(" << l->asValue() << ", " << r->asValue() << ")\n";
        delete l;
        delete r;
        return nullptr;
    }else{
        // incompleted code
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << op->return_type.ir_used << "(" << op->lhs_type->ir_used << ", " << op->rhs_type->ir_used << ")\n";
        delete l;
        delete r;
        return new Value(reg, op->return_type);
    }
    return nullptr;
}

Value *PrefixOperatorTerm::codegen(IRStream &f){
    auto r = rhs->codegen(f);
    if (op->return_type.name == "Void") {
        f << "call void(" << op->rhs_type->ir_used << ") " << op->func_name << "(" << r->asValue() << ")\n";
        delete r;
        return nullptr;
    }else{
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << op->return_type.ir_used << "(" << op->rhs_type->ir_used << ") " << op->func_name << "(" << r->asValue() << ")\n";
        delete r;
        return new Value(reg, op->return_type);
    }
    return nullptr;
}

unique_ptr<PrefixOperatorExpr> PrefixOperatorExpr::init(Operator *ope, unique_ptr<Expr> expr){
    auto ptr = make_unique<PrefixOperatorExpr>();
    ptr->location = expr->location;
    ptr->type = ope->return_type;
    ptr->op = ope;
    ptr->rhs = move(expr);
    return ptr;
}

unique_ptr<PrefixOperatorTerm> PrefixOperatorTerm::init(Operator *oper, unique_ptr<Term> term){
    auto ptr = make_unique<PrefixOperatorTerm>();
    ptr->location = term->location;
    ptr->type = oper->return_type;
    ptr->rhs = move(term);
    ptr->op = oper;
    return ptr;
}

unique_ptr<InfixOperatorTerm> InfixOperatorTerm::init(unique_ptr<Term> term, Operator *op, unique_ptr<Term> termr){
    auto ptr = make_unique<InfixOperatorTerm>();
    ptr->location = term->location;
    ptr->type = op->return_type;
    ptr->lhs = move(term);
    ptr->rhs = move(termr);
    ptr->op = op;
    return ptr;
}

Value *InfixOperatorPrimaryExpr::codegen(IRStream &f){
    auto l = lhs->codegen(f);
    auto r = rhs->codegen(f);
    if (op->return_type.name == "Void") {
        f << "call void (" << op->lhs_type->ir_used << ", " << op->rhs_type->ir_used << ") " << op->func_name << "(" << l->asValue() << ", " << r->asValue() << "\n";
        delete l;
        delete r;
        return nullptr;
    }else{
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << op->return_type.ir_used << "(" << l->type.ir_used << ", " << r->type.ir_used << ") " << op->func_name << "(" << l->asValue() << ", " << r->asValue() << ")\n";
        delete l;
        delete r;
        return new Value(reg, op->return_type);
    }
    return nullptr;
}

unique_ptr<PrefixOperatorPrimaryExpr> PrefixOperatorPrimaryExpr::init(Operator *op, unique_ptr<PrimaryExpr> primary){
    auto ptr = make_unique<PrefixOperatorPrimaryExpr>();
    ptr->location = primary->location;
    ptr->type = op->return_type;
    ptr->op = op;
    ptr->rhs = move(primary);
    return ptr;
}

Value *FunctionAttributeExpr::codegenAsRef(IRStream &f){
    auto value = codegen(f);
    if (!value) {
        return nullptr;
    }
    auto r = ScheatContext::local()->getRegister();
    f << r << " = alloca " << value->type.ir_used << "\n";
    f << "store " << value->asValue() << ", " << value->type.ir_used << "* " << r << "\n";
    return new Value(r, value->type.pointer());
}

Value *FunctionAttributeExpr::codegen(IRStream &f){
    string reg = "";
    
    if (args != nullptr) {
        values = args->codegenAsArray(f);
    }
    
    if (type == "Void") {
        f << "call " << func->lltype() << " " << func->getMangledName() << "(";
        return nullptr;
    }else{
        reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << func->lltype() << " " << func->getMangledName() << "(";
    }
    
    for (int i = 0; i < values.size(); i++) {
        f << values[i]->asValue();
        if (i != values.size() - 1) {
            f << ", ";
        }
    }
    
    f << ")\n";
    return new Value(reg, type);
}

Value *PrefixOperatorPrimaryExpr::codegen(IRStream &f){
    auto r = rhs->codegen(f);
    if (op->return_type.name == "Void") {
        f << "call " << op->return_type.ir_used << "(" << r->type.ir_used << ") " << op->func_name << "(" << r->asValue() << ")\n";
        delete r;
        return nullptr;
    }else{
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << op->return_type.ir_used << "";
        delete r;
        return new Value(reg, op->return_type);
    }
    return nullptr;
}


unique_ptr<PostfixOperatorPrimaryExpr> PostfixOperatorPrimaryExpr::init(unique_ptr<PrimaryExpr> primary, Operator *op){
    auto ptr = make_unique<PostfixOperatorPrimaryExpr>();
    ptr->type = op->return_type;
    ptr->location = primary->location;
    ptr->lhs = move(primary);
    return ptr;
}

unique_ptr<InfixOperatorPrimaryExpr> InfixOperatorPrimaryExpr::init(unique_ptr<Term> term, Operator *oper, unique_ptr<PrimaryExpr> primary){
    auto ptr = make_unique<InfixOperatorPrimaryExpr>();
    ptr->type = oper->return_type;
    ptr->location = term->location;
    ptr->lhs = move(term);
    ptr->rhs = move(primary);
    ptr->op = oper;
    return ptr;
}

//Value *NewIdentifierExpr::codegen(IRStream &f){
//    return nullptr;
//}

static int countof(string t, char c){
    int i = 0;
    
    for (int u = 0; u < t.size(); u++) {
        if (t[u] == c) {
            i++;
        }
        
    }
    return i;
}

Value *VariableTerm::codegen(IRStream &f){
    auto vp = ScheatContext::local()->findVariable(value);
    if (!vp) {
        scheato->FatalError(location, __FILE_NAME__, __LINE__,
                            "this error should be appeared by analyzer. (by Encoder)");
        return nullptr;
    }
    auto reg = ScheatContext::local()->getRegister();
    f << reg << " = load " << vp->type.ir_used << ", " << vp->type.ir_used << "* " << vp->mangledName << "\n";
    return new Value(reg, type);
}

unique_ptr<VariableTerm> VariableTerm::init(Token *tok, TypeData tp){
    auto ptr = make_unique<VariableTerm>();
    ptr->location = tok->location;
    ptr->type = tp;
    ptr->value = tok->value.strValue;
    return ptr;
}

string AllocationExpr::userdump(){
    return "allocate(" + expr->userdump() + ")";
}

Value *AllocationExpr::codegen(IRStream &f){
    auto val = expr->codegen(f);
    if (!val) {
        scheato->FatalError(expr->location, __FILE_NAME__, __LINE__,
                            "this expression is void.");
        return nullptr;
    }

    auto tp = val->type;
    auto cln = ScheatContext::local()->findClass(tp.name);
    if (!cln) {
        scheato->FatalError(expr->location, __FILE_NAME__, __LINE__,
                            "this expression's type is undefined.");
        return nullptr;
    }
    auto r = ScheatContext::local()->getRegister();
    f << r << " = call i8*(i64, void(i8*)*) @ScheatPointer_alloc(" << to_string(cln->size) << ", " << ((cln->destructor != nullptr) ? cln->destructor->asValue() : "void(i8*)* null") << ")\n";
    f << "store " << val->asValue() << ", " << val->type.ir_used + "*" << r << "\n";
    return new Value(r, cln->type->pointer());
}

Value *StringTerm::codegen(IRStream &f){
    
    string substr = value;
    
    strreplace(substr, "\\0", "\\00");
    strreplace(substr, "\\n", "\\0A");
    strreplace(substr, "\n", "\\0A");
    strreplace(substr, "\t", "\\09");
    strreplace(substr, "\\t", "\\09");
    strreplace(substr, "\\\"", "\\22");
    string r = "@str";
    if (ScheatContext::global->strmap.find(substr) == ScheatContext::global->strmap.end()) {
        r = r + to_string(ScheatContext::global->strmap.size());
    }else{
        r = r + to_string(ScheatContext::global->strmap[substr]);
    }
    auto rn = r;
    int strlength = value.size() - countof(value, '\\') - 2;
    auto sname =  rn;
    ScheatContext::global->stream_entry <<
    sname << " = private unnamed_addr constant [" <<
    to_string(strlength) << " x i8] c" << substr << "\n";
    string v = "call %String(i8*) @String_init(i8* getelementptr inbounds ([" + to_string(strlength) + " x i8], [" + to_string(strlength) + " x i8]* " + sname + ", i32 0, i32 0))";
    string rr = ScheatContext::local()->getRegister();
    ScheatContext::local()->stream_body << rr << " = " << v << "\n";
    return new Value(rr, TypeData::StringType);
}

Value *BoolTerm::codegen(IRStream &f){
    if (value) {
        return new Value(to_string(1),type);
    }else{
        return new Value(to_string(0),type);
    }
    return nullptr;
}

vector<Value *> ArgumentExpr::codegenAsArray(IRStream &f){
    if (self == nullptr && !container) {
        return {};
    }
    if (container == nullptr) {
        return {self->codegen(f)};
    }
    if (self == nullptr) {
        return {container->codegen(f)};
    }
    auto arr = container->codegenAsArray(f);
    arr.push_back(self->codegen(f));
    return arr;
}

Value *PrintStatement::codegen(IRStream &f){
    for (auto v : arg->codegenAsArray(f)) {
        if (v->type.name == TypeData::IntType.name) {
            f << "call void(i32) @print_i32(" << v->asValue() << ")\n";
        }else if (v->type.name == TypeData::StringType.name){
            f << "call void(%String) @print_String(" << v->asValue() << ")\n";
        }else{
            auto clasp = ScheatContext::global->findClass(v->type.name);
            if (!clasp) {
                scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__,
                                    "%s is unknown.", v->type.name.c_str());
                return nullptr;
            }
            auto func = clasp->context->findFunc("print");
            if (!func) {
                scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__,
                                    "this object can't print.");
            }
            f << "call void(" << v->type.mangledName() << ")" << func->getMangledName() << "(" << v->asValue() << ")\n";
        }
    }
    f << "call void() @printn()\n";
    return nullptr;
}

unique_ptr<ArgumentExpr> ArgumentExpr::init(unique_ptr<Expr> bdy){
    auto ptr = make_unique<ArgumentExpr>();
    if (bdy == nullptr) {
        ptr->location = SourceLocation();
        ptr->self = nullptr;
        ptr->type = TypeData("", "");
        return ptr;
    }
    ptr->location = bdy->location;
    ptr->type = bdy->type;
    ptr->self = move(bdy);
    return ptr;
}

unique_ptr<ArgumentExpr> ArgumentExpr::addArg(unique_ptr<ArgumentExpr> args, unique_ptr<Expr> n){
    auto ptr = make_unique<ArgumentExpr>();
    ptr->container = move(args);
    ptr->self = move(n);
    return ptr;
}

Value *VariableAttributeExpr::codegenWithParent(Value *parent, IRStream &f){
    string reg = ScheatContext::local()->getRegister();
    f << reg << " = getelementptr " << parent->type.ir_used << ", " << parent->asValue() << ", i32 0, i32 " << to_string(varindex.index) << "\n";
    return new Value(reg, type);
}

Value *FunctionAttributeExpr::codegenWithParent(Value *parent, IRStream &f){
    values.insert(values.begin(), parent);
    if (func->return_type == "Void") {
        f << "call " << func->lltype() << " " << func->getMangledName() << "(";
        for (auto vptr : values) {
            f << vptr->asValue();
        }
        f << ")\n";
        return nullptr;
    }else{
        string reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << func->lltype() << " " << func->getMangledName() << "(";
        for (auto vptr : values) {
            f << vptr->asValue();
        }
        f << ")\n";
        return new Value(reg, func->return_type);
    }
    return nullptr;
}

Value *VariableTerm::codegenAsRef(IRStream &f){
    auto vp = ScheatContext::local()->findVariable(value);
    if (!vp) {
        scheato->FatalError(location, __FILE_NAME__, __LINE__,
                            "this error should be appeared by analyzer. (by Encoder)");
        return nullptr;
    }
    return new Value(vp->mangledName, type.pointer());
}

Value *AccessIdentifierExpr::codegenAsRef(IRStream &f){
    auto v = parent->codegenAsRef(f);
    if (child != nullptr) {
        return child->codegenWithParent(v, f);
    }
    return v;
}

Value *TheIdentifierExpr::codegen(IRStream &f){
    return expr->codegenAsRef(f);
}

Value *TheIdentifierExpr::codegenAsRef(IRStream &f){
    return expr->codegenAsRef(f);
}

unique_ptr<TheIdentifierExpr> TheIdentifierExpr::init(Token *the_tok, unique_ptr<IdentifierExpr> ex){
    auto ptr = make_unique<TheIdentifierExpr>();
    ptr->location = the_tok->location;
    ptr->type = TypeData("the " + ex->type.name, ex->type.ir_used + "*");
    ptr->expr = move(ex);
    return ptr;
}

unique_ptr<NewIdentifierExpr> NewIdentifierExpr::init(SourceLocation loc, string val, TypeData *type = nullptr){
    auto ptr = make_unique<NewIdentifierExpr>();
    ptr->type = type;
    ptr->value = val;
    ptr->location = loc;
    return ptr;
}

void NewIdentifierExpr::setType(TypeData typ){
    if (type == nullptr) {
        *type = typ;
    }else{
        if (type->ir_used != typ.ir_used) {
            scheato->FatalError(location, __FILE_NAME__, __LINE__, "%s is defined as %s but assigned %s value",
                                value.c_str(), type->name.c_str(), typ.name.c_str());
        }
        
    }
};

string CastExpr::userdump(){
    return type.name + " *" + expr->userdump();
}

unique_ptr<CastExpr> CastExpr::init(TypeData type, unique_ptr<Expr> ptr){
    auto pt = make_unique<CastExpr>();
    pt->location = ptr->location;
    pt->type = type;
    pt->expr = move(ptr);
    return pt;
}

Value *CastExpr::codegen(IRStream &f){
    auto v = expr->codegen(f);
    if (v->type.ir_used == type.ir_used) {
        return v;
    }
    if (*--v->type.ir_used.end() == '*') {
        scheato->DevLog(location, __FILE_NAME__, __LINE__, "pointer cast");
        string reg = ScheatContext::local()->getRegister();
        
        f << reg << " = bitcast " << v->asValue() << " to " << type.ir_used << "*\n";
        return new Value(reg, type);
    }else{
        string reg = ScheatContext::local()->getRegister();
        f << reg << " = sext " << v->asValue() << " to " << type.ir_used << "\n";
        return new Value(reg, type);
    }
    return nullptr;
}

Value *NewIdentifierExpr::codegenAsRef(IRStream &f){
    string reg;
    
    if (type == nullptr) {
        scheato->FatalError(location, __FILE_NAME__, __LINE__, "type is not clear.");
        return nullptr;
    }
    
    auto classptr = ScheatContext::global->findClass(type->name);
    if (!classptr) {
        scheato->FatalError(location, __FILE_NAME__, __LINE__, "%s is defined as unknown type %s",
                            value.c_str(),
                            type->name.c_str());
        return nullptr;
    }
    auto func = classptr->context->findFunc(type->name + "_init");
    if (!func) {
        scheato->FatalError(location, __FILE_NAME__, __LINE__, "%s is not suitable object for Scheat. To use this here, you need to define %s_init function.",
                            type->name.c_str(),
                            type->name.c_str());
        return nullptr;
    }
    
    
    if (ScheatContext::local()->name == "main") {
        reg = "@" + value;
        ScheatContext::global->stream_entry << reg << " = global " << type->ir_used << " zeroinitializer\n";
    }else if (ScheatContext::local()->name == "Global"){
        reg = "@" + value;
        ScheatContext::global->stream_entry << reg << " = global " << type->ir_used << " zeroinitializer\n";
    }else{
        reg = "%" + value;
        f << reg << " = alloca " << type->ir_used << "\n";
    }
    ScheatContext::init->stream_body << "call void " << func->getMangledName() << "(" << type->ir_used << "* " << reg << ")\n";
    return new Value(reg, *type);
}

Value *AccessIdentifierExpr::codegen(IRStream &f){
    return codegenAsRef(f);
}

string AccessIdentifierExpr::userdump(){
    if (child == nullptr) {
        return parent->userdump();
    }else{
        return parent->userdump() + child->userdump();
    }
    return "";
}

unique_ptr<AccessIdentifierExpr>
AccessIdentifierExpr::init(unique_ptr<IdentifierExpr> p, unique_ptr<IdentifierTerm> c){
    auto ptr = make_unique<AccessIdentifierExpr>();
    ptr->type = c->type;
    ptr->location = c->location;
    ptr->parent = move(p);
    ptr->child = move(c);
    
    return ptr;
};

Value *IdentifierTerm::codegen(IRStream &f){
    scheato->DevLog(location, __FILE_NAME__, __LINE__, "this function must not be called.");
    exit(0);
    return nullptr;
}

unique_ptr<VariableAttributeExpr> VariableAttributeExpr::init(Property varptr, SourceLocation location){
    auto ptr = make_unique<VariableAttributeExpr>();
    ptr->varindex = varptr;
    ptr->location = location;
    ptr->type = varptr.type;
    return ptr;
}

unique_ptr<FunctionAttributeExpr> FunctionAttributeExpr::init(Function *func, unique_ptr<ArgumentExpr> args, SourceLocation location){
    auto ptr = make_unique<FunctionAttributeExpr>();
    ptr->location = location;
    ptr->func = func;
    ptr->type = func->return_type;
    ptr->args = move(args);
    return ptr;
}
Value *FloatTerm::codegen(IRStream &f){
    
    return new Value(to_string(value), type);
}

Value *IntTerm::codegen(IRStream &f){
    return new Value(to_string(value), type);
}

unique_ptr<DeclareVariableStatement>
DeclareVariableStatement::init(unique_ptr<NewIdentifierExpr> name, unique_ptr<Expr> expr, bool pub, bool con, bool nul){
    auto n = make_unique<DeclareVariableStatement>();
    n->name = name->value;
    n->isConstant = con;
    n->isNullable = nul;
    n->isPublic = pub;
    n->value = move(expr);
    n->isGlobal = name->isGlobal;
    name.release();
    return n;
}

Value *Statements::codegen(IRStream &f){
    if (statements != nullptr) statements->codegen(f);
    if (scheato->hasProbrem()) {
        return nullptr;
    }
    statement->codegen(f);
    return nullptr;
}

Value *DeclareVariableStatement::codegen(IRStream &f){
    
    if (scheato->hasProbrem()) {
        return nullptr;
    }
    
    if (isGlobal) {
        if (value->type.name == "Int") {
            ScheatContext::global->stream_body << name << " = global i32 0\n";
            string k = scheato->productName + "_init";
            auto ff = ScheatContext::global->findFunc(k);
            if (!ff) {
                scheato->DevLog(location, __FILE_NAME__, __LINE__, "_init function is not defined");
                return nullptr;
            }
            auto v = value->codegen(ff->context->stream_body);
            
            if (!v) {
                return nullptr;
            }
            
            ff->context->stream_body << "store i32 " << v->value << ", i32* " << name << "\n";
            return nullptr;
        }else if (value->type.name == "String"){
            ScheatContext::global->stream_entry << name << " = global zeroinitializer %String*\n";
            auto ff = ScheatContext::global->findFunc(scheato->productName + "_init");
            if (!ff) {
                scheato->DevLog(location, __FILE_NAME__, __LINE__, "_init function is not defined");
                return nullptr;
            }
            ScheatContext::push(ff->context);
            auto v = value->codegen(ff->context->stream_body);
            if (!v) {
                return nullptr;
            }
            ff->context->stream_body << "store %String " << v->value << ", %String* " << name << "\n";
            ScheatContext::pop();
            return nullptr;
        }
    }else{
        // it is local variable
        if (value->type.name == "Int") {
            f << name << " = alloca i32\n";
            auto v = value->codegen(f);
            
            if (!v) {
                return nullptr;
            }
            
            f << "store i32 " << v->value << ", i32* " << name << "\n";
            return nullptr;
        }else if (value->type.name == "String"){
            f << name << " = alloca  %String\n";
            auto v = value->codegen(f);
            if (!v) {
                return nullptr;
            }
            f << "store %String " << v->value << ", %String* " << name << "\n";
            return nullptr;
        }
        
    }
    return nullptr;
}
