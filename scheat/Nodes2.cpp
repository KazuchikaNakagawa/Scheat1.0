//
//  YANodes.cpp
//  scheat
//
//  Created by かずちか on 2020/09/26.
//

#include "Nodes2.h"
#include "ScheatStatics.h"
#include <regex>
#include "Utilities.h"
//#include <dlfcn.h>
using namespace scheat;
using namespace scheat::nodes2;
using namespace std;

Value *Class::getProperty(string s, Value *v, IRStream &f){
    auto pair = properties.find(s);
    if (pair == properties.end()) {
        
        if (!parentClass) {
            return nullptr;
        }
        auto r = ScheatContext::local()->getRegister();
        f << r << " = getelementptr " << type->ir_used << ", " << type->pointer().ir_used << " i32 0, i32 0\n";
        
        return parentClass->getProperty(s, new Value(r, *parentClass->type),f);
    }else{
        
    }
    
    return nullptr;
}


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
    return ty.pointer();
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

unique_ptr<FunctionCallTerm> FunctionCallTerm::init(Token *token, Function *func, unique_ptr<ArgumentExpr> a){
    auto ptr = make_unique<FunctionCallTerm>();
    ptr->func = func;
    ptr->value = token->value.strValue;
    ptr->args = move(a);
    ptr->value = "FUNCTION";
    ptr->type = func->return_type;
    return ptr;
}

Value *LoadExpr::codegen(IRStream &f){
    string r = ScheatContext::local()->getRegister();
    auto v = expr->codegen(f);
    f << r << " = load " << type.ir_used << ", " << v->asValue() << "\n";
    delete v;
    return new Value(r, type);
}

Value * ForStatement::codegen(IRStream &f){
    auto v = this->count->codegen(f);
    auto lbl = scopeName;
    auto condreg = "%" + lbl + "i";
    f << condreg << " = alloca i32\n";
    f << "store i32 0, i32* " << condreg << "\n";
    f << "br label %" << lbl << "for\n";
    f << lbl << "for:\n";
    auto r = ScheatContext::local()->getRegister();
    f << r << " = load i32, i32* " << condreg << "\n";
    auto re = ScheatContext::local()->getRegister();
    f << re << " = icmp slt i32 " << r << ", " << v->value << "\n";
    f << "br i1 " << re << ", label %" << lbl + "body, label %" << lbl + "for_end\n";
    f << lbl + "body:\n" ;
    ScheatContext::local()->entryScope(lbl + "for");
    body->codegen(f);
    ScheatContext::local()->leave();
    auto r1 = ScheatContext::local()->getRegister();
    auto r2 = ScheatContext::local()->getRegister();
    f << r1 << " = load i32, i32* " << condreg << "\n";
    f << r2 << " = add nsw i32 1, " << r1 << "\n";
    f << "store i32 " << r2 << ", i32* " << condreg << "\n";
    f << "br label %" << lbl + "for\n";
    f << lbl + "for_end:\n";
    return nullptr;
}

Value * WhileStatement::codegen(IRStream &f){
    auto lbl = scopeName;
    f << "br label %" << lbl << "while\n";
    f << lbl << "while:\n";
    auto re = condition->codegen(f);
    f << "br i1 " << re->value << ", label %" << lbl + "body, label %" << lbl + "while_end\n";
    f << lbl + "body:\n" ;
    ScheatContext::local()->entryScope(lbl + "while");
    body->codegen(f);
    ScheatContext::local()->leave();
    f << "br label %" << lbl + "while\n";
    f << lbl + "while_end:\n";
    return nullptr;
}

string LoadExpr::userdump(){
    return "get(" + expr->userdump() + ")";
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

Value *BreakStatement::codegen(IRStream &f){
//    auto scope = ScheatContext::local()->getNowScope();
//    if (scope.empty()) {
//        scheato->FatalError(location, __FILE_NAME__, __LINE__,
//                            "Compiler seem not tobe working well.");
//        return nullptr;
//    }
    f << "br label %" << scope + "_end\n";
    return nullptr;
}

Value *ContinueStatement::codegen(IRStream &f){
    //    auto scope = ScheatContext::local()->getNowScope();
    //    if (scope.empty()) {
    //        scheato->FatalError(location, __FILE_NAME__, __LINE__,
    //                            "Compiler seem not tobe working well.");
    //        return nullptr;
    //    }
    f << "br label %" << scope + "\n";
    return nullptr;
}

Value *ClassDefinitionStatement::codegen(IRStream &f){
    ScheatContext::global->stream_body << "%" << name << " = type{";
    for (auto pair : classObject->properties) {
        ScheatContext::global->stream_body << pair.second.type.ir_used << ", ";
    }
    
    for (auto pair : classObject->members) {
        ScheatContext::global->stream_body << pair.second->funcType() << ", ";
    }
    
    ScheatContext::global->stream_body.irs.pop_back();
    
    ScheatContext::global->stream_body << "}\n";
    
    statements->codegen(f);
    
    return nullptr;
}

Value *ClassDefinitionStatement::getProperty(Context * context, IRStream &f, string name){
    auto p = classObject->properties.find(name);
    if (p == classObject->properties.end()) {
        // in Scheat 1.0, we do NOT insert super class object, so
        // after Scheat2.0, We need to search super(getelementptr i32 0, i32 0)
        return nullptr;
    }
    
    return nullptr;
}

Node::Node(){
    context = ScheatContext::local();
}

Value *PropertyDeclareStatement::codegen(IRStream &f){
    auto v = initializedValue->codegen(host->constructor->context->stream_body);
    host->constructor->context->stream_body << "store " << v->asValue() << ", " << propertyType->ir_used << "* %" << name << "\n";
    if (propertyType->ir_used.find("*") == string::npos) {
        auto a = host->destructor->context;
        auto preg = a->getRegister();
        a->stream_body << preg << " = bitcast " << p->type.ir_used << "* %" << name << " to i8*\n";
        a->stream_body << "call void @" << p->type.name << "_deinit(i8* " << preg << ")\n";
        
    }else{
        /*
         pointer property is not sure yet
         */
        auto a = host->destructor->context;
        auto preg = a->getRegister();
        a->stream_body << preg << " = load " << p->type.ir_used << ", "
        << p->type.pointer().ir_used << " %" << name << "\n";
        auto sreg = a->getRegister();
        a->stream_body << sreg << " = bitcast " << p->type.ir_used << " %" << name << " to i8*\n";
        a->stream_body << "call void @ScheatPointer_unref(i8* " << sreg << ")\n";
    }
    
    return nullptr;
}

Value *FunctionCallTerm::codegen(IRStream &f){
    if (func->return_type.ir_used == "void") {
        //ScheatContext::push(func->context);
        
        vector<Value *> arges_val = args->codegenAsArray(f);
        
        //ScheatContext::pop();
        f << "call " << func->funcType() << " @" << func->name << "(";
        
        int c = 1;
        for (auto ptr : arges_val) {
            f << ptr->asValue();
            if (c < arges_val.size()) {
                f << ", ";
            }
            c++;
        }
        f << ")\n";
        return nullptr;
    }else{
        vector<Value *> arges_val = args->codegenAsArray(f);
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << func->funcType() << " @" << func->name << "(";
        int c = 1;
        for (auto ptr : arges_val) {
            f << ptr->asValue();
            if (c < arges_val.size()) {
                f << ", ";
            }
        }
        f << ")\n";
        return new Value(reg, func->return_type);
    }
    return nullptr;
}

Value *IfStatement::codegen(IRStream &f){
    auto condv = condition->codegen(f);
    auto labels = ScheatContext::local()->getLabel();
    f << "br " << condv->asValue() << ", label %" << labels + "if" << ", label %" << labels + "else" << "\n";
    f << labels + "if" << ":\n";
    thenS->codegen(f);
    f << "br label %" << labels + "if" + "_end\n";
    f << labels + "else" << ":\n";
    
    if (elseS != nullptr) elseS->codegen(f);
    f << "br label %" << labels + "if" + "_end\n";
    f << labels + "if" + "_end:\n";
    return nullptr;
}

Value *InfixOperatorExpr::codegen(IRStream &f){
    auto l = lhs->codegen(f);
    auto r = rhs->codegen(f);
    if (l->type == TypeData::IntType &&
        r->type == TypeData::IntType && op->return_type == TypeData::IntType) {
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = " << op->func_name << " nsw i32 " << l->value << ", " << r->value << "\n";
        return new Value(reg, TypeData::IntType);
    }
    if (l->type == TypeData::IntType
        && r->type == TypeData::IntType
        && op->return_type == TypeData::BoolType) {
        auto reg = ScheatContext::local()->getRegister();
        string eqopt = "";
        if (op->value == "==") {
            eqopt = "eq";
        }else if (op->value == "<"){
            eqopt = "ult";
        }else if (op->value == ">"){
            eqopt = "ugt";
        }else if (op->value == "!="){
            eqopt = "ne";
        }else if (op->value == "=<" || op->value == "<="){
            eqopt = "ule";
        }else if (op->value == ">="){
            eqopt = "uge";
        }
        
        f << reg << " = icmp " << eqopt << " " << l->asValue() << ", " << r->value << "\n";
        return new Value(reg, TypeData::BoolType);
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
    
    if (l->type == TypeData::IntType) {
        auto reg = ScheatContext::local()->getRegister();
        
        if (op->value == "*") {
            f << reg << " = mul nsw " << l->asValue() << ", " << r->value << "\n";
            return new Value(reg, TypeData::IntType);
        }else if (op->value == "/"){
            f << reg << " = fdiv nsw " << l->asValue() << ", " << r->asValue() << "\n";
            return new Value(reg, TypeData::IntType);
        }
    }
    
    if (op->return_type.name == "Void") {
        f << "call void" << " " << op->func_name << "(" << l->asValue() << ", " << r->asValue() << "\n";
        delete l;
        delete r;
        return nullptr;
    }else{
        auto reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << op->return_type.ir_used << " " << op->func_name << "(" << l->asValue() << ", " << r->asValue() << ")\n";
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
        f << "call " << func->funcType() << " " << func->getMangledName() << "(";
        return nullptr;
    }else{
        reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << func->funcType() << " " << func->getMangledName() << "(";
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
    f << r << " = call i8*(i64, void(i8*)*) @_ScheatPointer_alloc(" << to_string(cln->size) << ", " << ((cln->destructor != nullptr) ? cln->destructor->asValue() : "void(i8*)* null") << ")\n";
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
        ScheatContext::global->strmap[substr] = true;
    }else{
        r = r + to_string(ScheatContext::global->strmap[substr]);
    }
    auto rn = r;
    int strlength = value.size() - countof(value, '\\') - 2;
    auto sname =  rn;
    sname.insert(1, "_");
    ScheatContext::global->stream_entry <<
    sname << " = private unnamed_addr constant [" <<
    to_string(strlength) << " x i8] c" << substr << "\n";
    // 一度ロードして使わなくてはならない
    ScheatContext::global->stream_entry <<
    rn << " = global i8* getelementptr inbounds ([" + to_string(strlength) + " x i8], [" + to_string(strlength) + " x i8]* " + sname + ", i64 0, i64 0)\n";
    string reg = ScheatContext::local()->getRegister();
    f << reg << " = load i8*, i8** " << rn << "\n";
    string v = "call %String @String_init_pi8(i8* " + reg + ")";
    string rr = ScheatContext::local()->getRegister();
    f << rr << " = " << v << "\n";
    return new Value(rr, TypeData::StringType);
}

vector<TypeData> ArgumentExpr::getTypes(){
    if (!container && !self) {
        return {};
    }
    if (!container) {
        return {type};
    }
    auto c = container->getTypes();
    c.push_back(type);
    return c;
}



Value *DeclareFunctionStatement::codegen(IRStream &f){
    context->stream_entry << "define " << (context->type ? context->type->ir_used : "void") << " @" << name->name << "(";
    for (int i = 0; i < argTypes.size(); i++) {
        context->stream_entry << argTypes[i].ir_used << " %arg" << to_string(i);
        if (i != argTypes.size() - 1) {
            context->stream_entry << ", ";
        }
        context->stream_body << "%" << argNames[i] << " = alloca " << argTypes[i].ir_used << "\n";
        context->stream_body << "store " << argTypes[i].ir_used << " %arg" << to_string(i) << ", " << argTypes[i].ir_used << "* %" << argNames[i] << "\n";
    }
    context->stream_entry << "){\nentry:\n";
    ScheatContext::push(context);
    body->codegen(context->stream_body);
    ScheatContext::pop();
    //context->stream_body << "ret " << (context->type ? context->type->ir_used : "void") << "\n";
    if (!context->type || context->type->ir_used == "void") {
        context->stream_body << "ret void\n";
    }else if (context->type->ir_used.find("*") != string::npos){
        context->stream_body << context->type->ir_used << " null\n";
    }else{
        auto r = context->getRegister();
        context->stream_body << r << " = call " << context->type->ir_used << " @" <<
        context->type->name + "_init()\n";
        context->stream_body << "ret " << context->type->ir_used << " " << r << "\n";
    }
    context->stream_tail << "}\n";
    return nullptr;
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
            f << "call void @print_i32(" << v->asValue() << ")\n";
        }else if (v->type.name == TypeData::StringType.name){
            f << "call void @print_String(" << v->asValue() << ")\n";
        }else{
            auto clasp = ScheatContext::global->findClass(v->type.name);
            if (!clasp) {
                scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__,
                                    "%s is not printable.", v->type.name.c_str());
                return nullptr;
            }
            auto func = clasp->context->findFunc("print_" + v->type.ir_used, {v->type});
            if (!func) {
                scheato->FatalError(SourceLocation(), __FILE_NAME__, __LINE__,
                                    "this object can't print.");
            }
            f << "call void(" << v->type.mangledName() << ")" << func->getMangledName() << "(" << v->asValue() << ")\n";
        }
    }
    f << "call void @print_return()\n";
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
    ptr->type = n->type;
    ptr->container = move(args);
    ptr->self = move(n);
    return ptr;
}

Value *VariableAttributeExpr::codegenWithParent(Value *parent, IRStream &f){
    string reg = context->getRegister();
    f << reg << " = getelementptr " << parent->type.loaded().ir_used << ", " << parent->asValue() << ", i32 0, i32 " << to_string(varindex.index) << "\n";
    return new Value(reg, type);
}

Value *FunctionAttributeExpr::codegenWithParent(Value *parent, IRStream &f){
    values.insert(values.begin(), parent);
    if (func->return_type == "Void") {
        f << "call " << func->funcType() << " " << func->getMangledName() << "(";
        for (auto vptr : values) {
            f << vptr->asValue();
        }
        f << ")\n";
        return nullptr;
    }else{
        string reg = ScheatContext::local()->getRegister();
        f << reg << " = call " << func->funcType() << " " << func->getMangledName() << "(";
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
        auto v2 = child->codegenWithParent(v, f);
        v2->type = v2->type.pointer();
        return v2;
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
    ptr->type = ex->type.pointer();
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
    return type.name + " of " + expr->userdump();
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
    auto func = classptr->context->findFunc(type->name + "_init_" + type->ir_used, {*type});
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
    auto v = codegenAsRef(f);
    auto r = ScheatContext::local()->getRegister();
    f << r << " = load " << v->type.loaded().ir_used << ", " << v->asValue() << "\n";
    return new Value(r, v->type.loaded());
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
    ptr->type = TypeData(c->type.name, c->type.ir_used);
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
    n->type = *name->type;
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

unique_ptr<ReassignStatement>
ReassignStatement::init(unique_ptr<IdentifierExpr> id, unique_ptr<Expr> v){
    auto ptr = make_unique<ReassignStatement>();
    ptr->location = id->location;
    ptr->type = id->type;
    ptr->value = move(v);
    ptr->variable = move(id);
    return ptr;
}

Value *ReassignStatement::codegen(IRStream &f){
    auto lv = variable->codegenAsRef(f);
    auto rv = value->codegen(f);
    f << "store " << rv->asValue() << ", " << lv->asValue() << "\n";
    return nullptr;
}

Value *DeclareVariableStatement::codegen(IRStream &f){
    
    if (scheato->hasProbrem()) {
        return nullptr;
    }
    
    if (isGlobal) {
        if (value->type.name == "Int") {
            ScheatContext::global->stream_body << name << " = global i32 0\n";
            string k = getFileName(scheato->sourceFile) + "_init";
            auto ff = ScheatContext::global->findFunc(k, {});
            if (!ff) {
                scheato->DevLog(location, __FILE_NAME__, __LINE__, "_init function is not defined");
                return nullptr;
            }
            auto v = value->codegen(f);
            
            if (!v) {
                return nullptr;
            }
            
            f << "store i32 " << v->value << ", i32* " << name << "\n";
            return nullptr;
        }else if (value->type.name == "String"){
            ScheatContext::global->stream_entry << name << " = common global %String zeroinitializer\n";
            auto ff = ScheatContext::global->findFunc(getFileName(scheato->sourceFile) + "_init", {});
            if (!ff) {
                scheato->FatalError(location, __FILE_NAME__, __LINE__, "_init function is not defined");
                return nullptr;
            }
            ScheatContext::push(ff->context);
            string r = ScheatContext::local()->getRegister();
            ff->context->stream_body << r << " = call %String @String_init()\n";
            ff->context->stream_body << "store %String " << r << ", %String* " << name << "\n";
            
            ScheatContext::pop();
            auto v = value->codegen(f);
            if (!v) {
                return nullptr;
            }
            f << "store %String " << v->value << ", %String* " << name << "\n";
            return nullptr;
        }else if (value->type.name == "Bool") {
            ScheatContext::global->stream_entry << name << " = global i1 0";
            ScheatContext::push(ScheatContext::init);
            auto v = value->codegen(ScheatContext::local()->stream_body);
            if (!v) {
                scheato->FatalError(location, __FILE_NAME__, __LINE__, "expression is void and failed to initialize.");
                return nullptr;
            }
            ScheatContext::local()->stream_body << "store " << v->asValue() << ", i1* " << name << "\n";
            return nullptr;
        }else{
            ScheatContext::global->stream_entry << name << " = common global  " << type.ir_used << " zeroinitializer\n";
            auto v = value->codegen(ScheatContext::init->stream_body);
            ScheatContext::init->stream_body << "store " << v->asValue() << ", " << type.ir_used << "* " << name << "\n";
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
//        }else if (value->type.name == "Bool") {
//            f << name << " = alloca i1\n";
            
        }else{
            ScheatContext::global->stream_entry << name << " = alloca " << type.ir_used << "*\n";
            auto v = value->codegen(ScheatContext::init->stream_body);
            ScheatContext::init->stream_body << "store " << v->asValue() << ", " << type.ir_used << "* " << name << "\n";
            return nullptr;
        }
        
    }
    return nullptr;
}
