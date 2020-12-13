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

#endif /* ScheatContextStructures_h */
