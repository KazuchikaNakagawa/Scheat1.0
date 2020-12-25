//
//  ScheatTypes.h
//  ProjectScheat
//
//  Created by かずちか on 2020/12/13.
//

#ifndef ScheatTypes_h
#define ScheatTypes_h
#include <string>

namespace scheat {
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
    
    TypeData pointer(){
        return TypeData("the " + name, ir_used + "*");
    };
    
    static TypeData IntType;
    static TypeData StringType;
};
}

#endif /* ScheatTypes_h */
