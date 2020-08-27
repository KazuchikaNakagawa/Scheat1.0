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
namespace scheat {

class Context;

namespace basicStructs{

class TypeData {
    
public:
    std::string ir_used;
    std::string name;
    
    bool operator==(TypeData *rhs){
        return this->name == rhs->name
        && this->mangledName() == rhs->mangledName();
    }
    std::string mangledName() const{ return ir_used; };
    TypeData(std::string nm){
        name = nm;
        ir_used = "%" + nm;
    }
};

class Operator {
    
public:
    enum{
        prefix,
        infix,
        postfix
    } position;
    
    enum{
        primary,
        secondary,
        term
    } precidence;
    
    TypeData return_type;
    // if postfix operator, this will be nullptr
    TypeData *rhs_type;
    // if prefix operator, this will be nullptr
    TypeData *lhs_type;
    std::string func_name;
};

class Function{
    
public:
    std::string mangledName;
    std::string getName();
    TypeData return_type;
    std::vector<TypeData> argTypes;
    std::string lltype();
    Context *context;
    Function(std::string ,std::string);
};

class Variable {
    
public:
    std::string mangledName;
    // do NOT think about pointer. it shows what value it has.
    TypeData type;
    Variable(std::string s, TypeData t) : mangledName(s), type(t) {
        
    }
};

class Property {
    
public:
    unsigned int index;
    TypeData type;
    Property(TypeData ty) : type(ty) { index = 0; };
};

class Class {
    std::map<std::string, unsigned int> properties;
    std::map<std::string, Operator> operators;
    unsigned int propCount = 0;
public:
    TypeData *type;
    Context *context;
    Class(TypeData *ty);
};
}

}

#endif /* BasicStructures_h */
