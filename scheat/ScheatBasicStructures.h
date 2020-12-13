//
//  BasicStructures.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/26.
//

#ifndef BasicStructures_h
#define BasicStructures_h

#include <map>
#include <vector>
#include "ScheatObjects.h"
#include "ScheatToken.h"
#include <stack>
namespace scheat {

class Context;

namespace statics{
class ScheatContext {
    static stack<Context *> localcon;
public:
    static vector<Context *> contextCenter;
    static void Init(Scheat *);
    static void AddMain();
    static void Shutdown(){ //delete ScheatContext::global;
        
    };
    static Context *main;
    static Context *global;
    static Context *init;
    static Context *local();
    static void push(Context *c) { localcon.push(c); };
    static void pop() { localcon.pop(); };
    static void printout();
};
}

namespace basics{

class IRStream {
public:
    std::vector<std::string> irs;
    IRStream& operator <<(std::string v){
        irs.push_back(v);
        return *this;
    };
    IRStream& operator <<(const char v[]){
        std::string vs(v);
        irs.push_back(vs);
        return *this;
    }
    void exportTo(std::ofstream &f);
    void printout();
    IRStream(){
        irs = {};
    };
};

class TypeData {
    
public:
    std::string ir_used;
    std::string name;
    
    bool operator==(TypeData *rhs){
        return this->name == rhs->name
        && this->mangledName() == rhs->mangledName();
    }
    bool operator==(std::string s){
        return this->name == s || this->mangledName() == s;
    }
    bool operator!=(TypeData *rhs){
        return this->name != rhs->name
        || this->mangledName() != rhs->mangledName();
    }
    std::string mangledName() const{ return ir_used; };
    TypeData(std::string nm){
        name = nm;
        
        char *n_b = (char *)malloc(sizeof(nm) + 1);
        int i = 0;
        strcpy(n_b, nm.c_str());
        if (nm == "double") {
            ir_used = "double";
            return;
        }
        else if (sscanf(n_b, "i%d", &i) == 1) {
            ir_used = nm;
            free(n_b);
            return;
        }else{
            free(n_b);
        }
        ir_used = "%" + nm;
        
    }
    
    TypeData() {};
    
    TypeData(std::string a, std::string ir) : name(a), ir_used(ir) {};
    
    
    static TypeData IntType;
    static TypeData StringType;
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
    Operator(std::string func, std::string symbols) : func_name(symbols), value(func){
        
    }
    Operator(){
        
    }
};

class Function{
    std::string mangledName;
public:
    
    std::string name;
    std::string getMangledName();
    TypeData return_type;
    std::vector<TypeData> argTypes;
    std::string lltype();
    /// basically it won't be used
    std::string codegen(IRStream &);
    Context *context;
    Function(std::string ,std::string);
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

class Class {
    
    unsigned int propCount = 0;
public:
    std::map<std::string, Property> properties;
    std::map<std::string, Operator*> operators;
    std::vector<TypeData *> bitMap;
    Class *parentClass;
    TypeData *type;
    Context *context;
    Operator *findOperator(string);
    Class(TypeData *ty);
};
}

}

#endif /* BasicStructures_h */
