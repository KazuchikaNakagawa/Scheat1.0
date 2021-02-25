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
    
    bool operator==(TypeData rhs){
        return this->name == rhs.name
        && this->mangledName() == rhs.mangledName();
    }
    bool operator==(std::string s){
        return this->name == s || this->mangledName() == s;
    }
    bool operator!=(TypeData *rhs){
        return this->name != rhs->name
        || this->mangledName() != rhs->mangledName();
    }
    std::string mangledName(){
        if (ir_used.find("*") != std::string::npos){
            return "p" + loaded().mangledName();
        }
        return ir_used;
    };
    
    TypeData(std::string nm){
        name = nm;
        
        char *n_b = (char *)malloc(sizeof(nm) + 1);
        int i = 0;
        strcpy(n_b, nm.c_str());
        if (nm == "double") {
            ir_used = "double";
            free(n_b);
            return;
        }
        else if (sscanf(n_b, "i%d", &i) == 1) {
            ir_used = nm;
            free(n_b);
            return;
        }else if (nm == "void"){
            ir_used = "void";
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
        //printf("pointer of %s\n", name.c_str());
        if (name.find("the ") != std::string::npos) {
            return TypeData(name + "*", ir_used + "*");
        }
        return TypeData("the " + name, ir_used + "*");
    };
    
    TypeData loaded(){
        auto k = name.find("*");
        if ( k != std::string::npos) {
            std::string cpy = name;
            cpy.erase(k);
            std::string cpy_i = ir_used;
            cpy_i.erase(cpy_i.find("*"));
            return TypeData(cpy, cpy_i);
        }
        k = name.find("the ");
        if (k != std::string::npos){
            std::string cpy = name;
            for (int i = 0; i < 4; i++) {
                cpy.erase(cpy.begin());
            }
            std::string cpy_i = ir_used;
            cpy_i.erase(cpy_i.find("*"));
            return TypeData(cpy,cpy_i);
        }
        return *this;
    }
    
    static TypeData IntType;
    static TypeData StringType;
    static TypeData BoolType;
};
}

#endif /* ScheatTypes_h */
