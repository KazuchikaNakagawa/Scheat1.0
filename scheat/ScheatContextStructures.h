//
//  ScheatContextStructures.h
//  ProjectScheat
//
//  Created by かずちか on 2020/12/13.
//

#ifndef ScheatContextStructures_h
#define ScheatContextStructures_h
#include <vector>
#include <string>
#include <map>
#include "ScheatTypes.h"

namespace scheat {
class Context;
using namespace std;

union IRObj {
    Context *con;
    string str;
    IRObj(Context *c){
        // do nothing
        con = c;
    }
    IRObj(string s){
        str = s;
    }
    ~IRObj(){
        // waiwai
    }
    IRObj(IRObj &&ref) : con(ref.con){
        con = ref.con;
    }
    IRObj(const IRObj&){
        
    }
};

struct IR {
    IRObj obj;
    bool isContext;
    IR(Context *con) : obj(con){
        isContext = true;
    };
    IR(string s) : obj(s){
        isContext = true;
    }
    IR(const IR &cr) : obj(cr.obj.con){
        if (cr.isContext) {
            obj.con = cr.obj.con;
        }else{
            obj.str = cr.obj.str;
        }
    }
    
    //IR(IR &&r) : obj(r.obj) = default;
//    {
//        if (r.isContext) {
//            obj.con = r.obj.con;
//        }else{
//            obj.str = r.obj.str;
//        }
//    }
};

class IRStream {
public:
    
    vector<IR> irs;
    IRStream& operator <<(std::string v){
        
        irs.push_back(v);
        return *this;
    };
    IRStream& operator <<(const char v[]){
        std::string vs(v);
        irs.push_back(vs);
        return *this;
    }
    IRStream& operator <<(TypeData t){
        return *this << t.ir_used;
    }
    IRStream& operator <<(int i){
        return *this << to_string(i);
    }
    IRStream& operator << (Context *c){
        irs.push_back(c);
        return *this;
    }
    void exportTo(std::ofstream &f);
    void printout();
    IRStream(){
        //irs = {};
    };
};

enum OperatorPosition {
    prefix,
    infix,
    postfix
};

enum OperatorPresidence {
    primary,
    secondary,
    termly
};

class Operator {
    
public:
    OperatorPosition position;
    
    OperatorPresidence precidence;
    
    std::string value;
    
    TypeData return_type;
    // if postfix operator, this will be nullptr
    TypeData *rhs_type;
    // if prefix operator, this will be nullptr
    TypeData *lhs_type;
    std::string func_name;
    int index = 0;
    Operator(std::string func, std::string symbols) : func_name(symbols), value(func){
        
    }
    Operator(){
        
    }
};

class Function{
    std::string mangledName;
public:
    
    std::string name;
    int index = 0;
    virtual std::string getMangledName();
    TypeData return_type;
    std::vector<TypeData> argTypes;
    std::string lltype();
    /// basically it won't be used
    std::string codegen(IRStream &);
    Context *context;
    Function(TypeData ,std::string,bool demangle = true);
    string asValue();
    bool isMemberFunction = false;
    bool ifTypesAdjust(vector<TypeData> argTy){
        if (argTypes.size() != argTy.size()) {
            return false;
        }
        for (int i = 0; i < argTypes.size(); i++) {
            if (argTypes[i].ir_used != argTy[i].ir_used) {
                return false;
            }
            
        }
        return true;
    }
    
    TypeData asPointer(){
        string a = "the " + return_type.name + "(";
        string ir = return_type.ir_used + "(";
        for (auto t : argTypes) {
            a += t.name + ",";
            ir += t.ir_used + ",";
        }
        a.pop_back();
        ir.pop_back();
        a += ")";
        ir += ")*";
        return TypeData(a, ir);
    }
    
};

// Grammar: enable to <function name>
class ExternalFunction : public Function {
public:
    string getMangledName() {
        return "@" + name;
    }
    ExternalFunction(TypeData _return_type, string fullName)
    : Function(_return_type, fullName, false)
    {
        
    };
};

class Variable {
    
public:
    std::string mangledName;
    int index = 0;
    // do NOT think about pointer. it shows what value it has.
    TypeData type;
    Variable(std::string s, TypeData t) : mangledName(s), type(t) {
        
    }
};

class Property {
    
public:
    unsigned int index;
    TypeData type;
    Property(TypeData ty = TypeData("the Void", "i8*")) : type(ty) { index = 0; };
};
namespace nodes2{
class Value;
}
class Class {
    
    unsigned int propCount = 0;
public:
    std::map<std::string, Property> properties;
    map<string, Function *> members;
    std::map<std::string, Operator*> operators;
    std::vector<TypeData> bitMap = {};
    Class *parentClass;
    TypeData *type;
    Context *context;
    nodes2::Value *getProperty(string, nodes2::Value *, IRStream &);
    Operator *findOperator(string);
    unsigned int size = 0;
    void addProperty(string key,Property p){
        p.index = propCount;
        properties[key] = p;
        bitMap.push_back(p.type);
        propCount++;
        size += p.type.size;
    }
    void addMemberFunc(string key, Function *f){
        f->index = propCount;
        f->isMemberFunction = true;
        members[key] = f;
        propCount++;
        bitMap.push_back(f->asPointer());
        size += 8;
    };
    void addOperator(string key, Operator *op){
        op->index = propCount;
        operators[key] = op;
        size += 8;
        propCount ++;
    }
    Class(TypeData *ty);
    Function *destructor = nullptr;
    Function *constructor = nullptr;
    Function *copyConstructor = nullptr;
    Function *assignedConstructor = nullptr;
};
}

#endif /* ScheatContextStructures_h */
