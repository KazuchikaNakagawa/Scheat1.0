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
    string v = "bitcast (i8* getelementptr inbounds ([" + to_string(strlength) + " x i8], [" + to_string(strlength) + " x i8]* " + sname + ", i32 0, i32 0) to %String*)";
    return new Value(v, this->type);
}

unique_ptr<Term> Term::init(unique_ptr<TermNode> up){
    auto u = make_unique<Term>();
    u->lhs = move(up);
    u->type = u->lhs->type;
    u->location = u->lhs->location;
    u->type = u->lhs->type;
    return u;
}

unique_ptr<Term> Term::initAsInfixOperatorExpr(unique_ptr<TermNode> lhs, Operator *oper, unique_ptr<Term> rhs){
    auto u = make_unique<Term>();
    u->type = oper->return_type;
    u->location = lhs->location;
    u->lhs = move(lhs);
    u->rhs = move(rhs);
    u->op = oper;
    return u;
}

unique_ptr<Term> Term::initAsPrefixOperatorExpr(Operator *oper, unique_ptr<Term> expr){
    auto u = make_unique<Term>();
    u->location = expr->location;
    u->op = oper;
    u->type = oper->return_type;
    u->rhs = move(expr);
    return u;
}

unique_ptr<Term> Term::initAsPostfixOperatorExpr(unique_ptr<Term> expr, Operator *oper){
    auto u = make_unique<Term>();
    u->location = expr->location;
    u->type = oper->return_type;
    u->op = oper;
    u->rhs = move(expr);
    return u;
}

Value *Term::codegen(IRStream &f){
    if (op != nullptr) {
        // this means (X op X), (op X), (X op)
        if (op->position == op->infix) {
            // infix operator
            auto r = local_context.top()->getRegister();
            auto lhsv = lhs->codegen(f);
            auto rhsv = rhs->codegen(f);
            if (!lhsv || !rhsv) {
                return nullptr;
            }
            
            f << r << " = call " << op->return_type.ir_used << "("
            << lhsv->type.ir_used << "*, " << rhsv->type.ir_used << "*) " << op->func_name << "(" << lhsv->type.ir_used
            << "* " << lhsv->value << ", " << rhsv->type.ir_used
            << "* " << rhsv->value << ")\n";
            return new Value(r, op->return_type);
        }else if (op->position == op->prefix){
            // prefix operator
            auto r = local_context.top()->getRegister();
            auto rhsv = rhs->codegen(f);
            if (!rhsv) {
                return nullptr;
            }
            f << r << " call " << op->return_type.ir_used << "("
            << rhsv->type.ir_used << "*) " << op->func_name << "("
            << rhsv->type.ir_used << "* " << rhsv->value << ")\n";
            return new Value(r, op->return_type);
        }else if (op->position == op->postfix){
            // postfix operator
            auto r = local_context.top()->getRegister();
            auto lhsv = rhs->codegen(f);
            if (!lhsv) {
                return nullptr;
            }
            f << r << " = call " << op->return_type.ir_used << "("
            << lhsv->type.ir_used << ") " << op->func_name << "("
            << lhsv->type.ir_used << "* " << lhsv->value << ")\n";
            return new Value(r, op->return_type);
        }else{
            scheato->DevLog(__FILE_NAME__, __LINE__, "?????");
            return nullptr;
        }
    }
    return lhs->codegen(f);
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
        if (funcptr == nullptr) {
            scheato->DevLog(__FILE_NAME__, __LINE__,
                            "compiler error. illegal function was set.");
            return nullptr;
        }
        string mangled = "";
        string mtype = type.name + " (";
        string mtype_ir = type.ir_used + " (";
        mangled += value;
        for (auto v = args.begin();
             v != args.end();
             mtype += ", ",
             mtype_ir += ", ",
             mangled += ", ",
             v = next(v)) {
            auto vv = (*v)->codegen(f);
            mangled += (vv)->type.ir_used + "* ";
            mangled += vv->value;
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
    if (args.size() > countOfArgs) {
        scheato->FatalError(__FILE_NAME__, __LINE__, "in %d.%d function %s was defined as the function which has %d arguments, but given %d arguments.",
                            location.line,
                            location.column,
                            this->value.c_str(),
                            countOfArgs,
                            args.size());
    }
}

string IdentifierTerm::userdump(){
    string ss = value;
    if (isFunc) {
        ss = "function(" + value + ")";
    }
    for (auto v = args.begin(); v != args.end(); v = next(v)) {
        ss = ss + (*v)->userdump();
    }
    return ss;
}

IdentifierTerm::IdentifierTerm(Token *t, int c, TypeData ty){
    type = ty;
    value = t->value.strValue;
    countOfArgs = c;
    location = t->location;
}

IdentifierExpr::IdentifierExpr(unique_ptr<IdentifierTerm> term,TypeData ty){
    rhs = move(term);
    location = rhs->location;
    type = ty;
}
IdentifierExpr::IdentifierExpr(unique_ptr<IdentifierExpr> expr, Token *tok, unique_ptr<IdentifierTerm> term, TypeData ty){
    lhs = move(expr);
    rhs = move(term);
    perTok = tok;
    type = ty;
    location = lhs->location;
}



Value *IdentifierExpr::codegenAsRef(IRStream &f){
    // idexpr : idterm
    //        | idexpr . idterm
    
    if (perTok == nullptr && lhs == nullptr) {
        // idexpr : idterm
        
        return rhs->codegen(f);
    }
    
    if (perTok == nullptr) {
        // idexpr : idexpr . idterm
        auto lhv = lhs->codegen(f);
        //auto rhv = rhs->codegen(f);
        auto r = local_context.top()->getRegister();
        f << r << " = getelementptr " << lhv->type.ir_used << ", " <<
        lhv->type.ir_used << "* " << lhv->value << ", i32 0, i32 " << to_string(rhs->index) << "\n";
        
    }
    
    return nullptr;
}

Value *IdentifierExpr::codegen(IRStream &f){
    auto v = codegenAsRef(f);
    if (!v) {
        scheato->FatalError(__FILE_NAME__, __LINE__,
                            "in %d.%d unable to access void value",
                            location.line,
                            location.column);
        return nullptr;
    }
    auto r = local_context.top()->getRegister();
    f << r << " = load " << v->type.ir_used << ", " << v->type.ir_used
    << "* " << v->value << "\n";
    return new Value(r, v->type);
}

unique_ptr<DeclareVariableStatement>
DeclareVariableStatement::init(Token *idtok, unique_ptr<Expr> expr, bool pub, bool con, bool nul){
    auto n = make_unique<DeclareVariableStatement>();
    n->name = idtok->value.strValue;
    n->isConstant = con;
    n->isNullable = nul;
    n->isPublic = pub;
    n->value = move(expr);
    return n;
}
