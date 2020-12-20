//
//  YANodes.cpp
//  scheat
//
//  Created by かずちか on 2020/09/26.
//

#include "Nodes2.h"
#include "ScheatStatics.h"
//#include <dlfcn.h>
using namespace scheat;
using namespace scheat::nodes2;
using namespace std;

static TypeData asPointer(TypeData ty){
    return TypeData("the " + ty.name, ty.ir_used + "*");
}

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

//Value* AccessIdentifierTerm::codegen(IRStream &f){
//    auto l = lhs->codegenAsRef(f);
//    auto r = rhs->codegen(f);
//    if (rhs->value == "FUNCTION") {
//        rhs->addArgument(true, move(lhs));
//
//    }else{
//        auto reg = local_context.top()->getRegister();
//        f << reg << " = getelementptr " << l->type.ir_used << ", " << l->asValue() << ", i32 0, i32 "
//        << to_string(index) << "\n";
//        return new Value(reg, r->type);
//    }
//    return nullptr;
//}

//unique_ptr<TheIdentifierTerm> TheIdentifierTerm::init(unique_ptr<IdentifierExprTemplate> expr){
//    auto ptr = make_unique<TheIdentifierTerm>();
//    ptr->location = expr->location;
//    ptr->type = TypeData(expr->type.name + "*", expr->type.name + "*");
//    ptr->id = move(expr);
//    return ptr;
//}
//
//Value *TheIdentifierTerm::codegen(IRStream &f){
//    return id->codegenAsRef(f);
//}
//
//unique_ptr<VariableTerm> VariableTerm::init(Token *id, TypeData type){
//    auto ptr = make_unique<VariableTerm>();
//    ptr->type = type;
//    ptr->value = id->value.strValue;
//    ptr->location = id->location;
//    return ptr;
//}
//
//unique_ptr<AccessIdentifierTerm> AccessIdentifierTerm::init(unique_ptr<Expr> expr, unique_ptr<IdentifierTermTemplate> id, int index){
//    auto ptr = make_unique<AccessIdentifierTerm>();
//    ptr->index = index;
//    ptr->lhs = move(expr);
//    ptr->type = id->type;
//    ptr->location = id->location;
//    ptr->rhs = move(id);
//    return ptr;
//}

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
    if (op->return_type.name == "Void") {
        f << "call void(" << op->lhs_type->ir_used << ", " << op->rhs_type->ir_used << ") " << op->func_name << "(" << l->asValue() << ", " << r->asValue() << ")\n";
        delete l;
        delete r;
        return nullptr;
    }else{
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

Value *FunctionAttributeExpr::codegen(IRStream &f){
    string reg = "";
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

Value *StringTerm::codegen(IRStream &f){
    
    string substr = value;
    
    strreplace(substr, "\\0", "\\00");
    strreplace(substr, "\\n", "\\0A");
    strreplace(substr, "\n", "\\0A");
    strreplace(substr, "\t", "\\09");
    strreplace(substr, "\\t", "\\09");
    strreplace(substr, "\\\"", "\\22");
    string r = "@";
    if (ScheatContext::global->strmap.find(substr) == ScheatContext::global->strmap.end()) {
        r = r + to_string(ScheatContext::global->strmap.size());
    }else{
        r = r + to_string(ScheatContext::global->strmap[substr]);
    }
    auto rn = r;
    int strlength = substr.size() - 2;
    auto sname =  rn + ".str";
    ScheatContext::global->stream_entry <<
    sname << " = private unnamed_addr constant [" <<
    to_string(strlength) << " x i8] c" << value << "\n";
    string v = "bitcast (i8* getelementptr inbounds ([" + to_string(strlength) + " x i8], [" + to_string(strlength) + " x i8]* " + sname + ", i32 0, i32 0) to %String*)";
    return new Value(v, this->type);
}

//unique_ptr<OperatedExpr> Expr::initAsSyntaxedExpr(unique_ptr<Expr> node){
//    auto u = make_unique<Expr>();
//    u->syntax = move(node);
//    u->syntaxedExpr = true;
//    return u;
//}

//Value *Expr::codegen(IRStream &f){
//    if (syntaxedExpr) {
//        return syntax->codegen(f);
//    }
//    if (op != nullptr) {
//        // this means (X op X), (op X), (X op)
//        if (op->position == op->infix) {
//            // infix operator
//            auto r = local_context.top()->getRegister();
//            auto lhsv = lhs->codegen(f);
//            auto rhsv = rhs->codegen(f);
//            if (!lhsv || !rhsv) {
//                return nullptr;
//            }
//
//            f << r << " = call " << op->return_type.ir_used << "("
//            << lhsv->type.ir_used << ", " << rhsv->type.ir_used << ") " << op->func_name << "(" << lhsv->type.ir_used
//            << " " << lhsv->value << ", " << rhsv->type.ir_used
//            << " " << rhsv->value << ")\n";
//            return new Value(r, op->return_type);
//        }else if (op->position == op->prefix){
//            // prefix operator
//            auto r = local_context.top()->getRegister();
//            auto rhsv = rhs->codegen(f);
//            if (!rhsv) {
//                return nullptr;
//            }
//            f << r << " call " << op->return_type.ir_used << "("
//            << rhsv->type.ir_used << ") " << op->func_name << "("
//            << rhsv->type.ir_used << " " << rhsv->value << ")\n";
//            return new Value(r, op->return_type);
//        }else if (op->position == op->postfix){
//            // postfix operator
//            auto r = local_context.top()->getRegister();
//            auto lhsv = rhs->codegen(f);
//            if (!lhsv) {
//                return nullptr;
//            }
//            f << r << " = call " << op->return_type.ir_used << "("
//            << lhsv->type.ir_used << ") " << op->func_name << "("
//            << lhsv->type.ir_used << " " << lhsv->value << ")\n";
//            return new Value(r, op->return_type);
//        }else{
//            scheato->DevLog(location, __FILE_NAME__, __LINE__, "?????");
//            return nullptr;
//        }
//    }
//    return lhs->codegen(f);
//}

Value *BoolTerm::codegen(IRStream &f){
    if (value) {
        return new Value(to_string(1),type);
    }else{
        return new Value(to_string(0),type);
    }
    return nullptr;
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

Value *AccessIdentifierExpr::codegenAsRef(IRStream &f){
    auto v = parent->codegen(f);
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
    string reg = ScheatContext::local()->getRegister();
    auto v = codegenAsRef(f);
    f << reg << " = load " << v->type.ir_used << ", " << v->asValue() << "\n";
    return new Value(reg, v->type);
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

unique_ptr<FunctionAttributeExpr> FunctionAttributeExpr::init(Function *func, SourceLocation location){
    auto ptr = make_unique<FunctionAttributeExpr>();
    ptr->location = location;
    ptr->func = func;
    ptr->type = func->return_type;
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
    if (ScheatContext::local()->name == "main") {
        // global function
        
        if (value->type.name == "Int") {
            ScheatContext::global->stream_entry << "@" << name << " = global i32 0\n";
            auto ff = ScheatContext::global->findFunc(ScheatContext::global->name + "_init");
            if (!ff) {
                scheato->DevLog(location, __FILE_NAME__, __LINE__, "_init function is not defined");
                return nullptr;
            }
            ScheatContext::push(ff->context);
            auto v = value->codegen(ff->context->stream_body);
            
            ff->context->stream_body << "store i32 " << v->value << ", i32* " << "@" << name << "\n";
            ScheatContext::pop();
            return nullptr;
            
        }else if (value->type.name == "String"){
            
        }
        
    }else if (ScheatContext::local()->name == "global"){
        if (value->type.name == "Int") {
            f << "@" << name << " = global i32 0\n";
            string k = scheato->productName + "_init";
            auto ff = ScheatContext::global->findFunc(k);
            if (!ff) {
                scheato->DevLog(location, __FILE_NAME__, __LINE__, "_init function is not defined");
                return nullptr;
            }
            auto v = value->codegen(ff->context->stream_body);
            ff->context->stream_body << "store i32 " << v->value << ", i32* " << "@" << name << "\n";
            return nullptr;
        }else if (value->type.name == "String"){
            
        }
    }
    return nullptr;
}
