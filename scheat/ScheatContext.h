//
//  ScheatContext.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/26.
//

#ifndef ScheatContext_h
#define ScheatContext_h
#include "ScheatBasicStructures.h"
#include <map>
#include <vector>
namespace scheat{

using namespace basics;

class Context {
    unsigned int rnum;
    std::map<std::string, Variable *> variables;
    std::map<std::string, Function *> funcs;
    std::map<std::string , Class *> classes;
    Context(std::string name, Context *parents){
        variables = {};
        funcs = {};
        rnum = 0;
        base = parents;
        this->name = name;
    }
public:
    IRStream stream_entry;
    IRStream stream_body;
    IRStream stream_tail;
    std::string name;
    Context *base;
    Variable *findVariable(std::string);
    Class *findClass(std::string);
    Function *findFunc(std::string);
    bool isExists(std::string);
    std::string getRegister();
    Context(){
        variables = {};
        funcs = {};
        rnum = 0;
        base = nullptr;
        name = "";
    }
    
    void addFunction(std::string, Function *);
    
    void addClass(std::string, Class *);
    
    void addVariable(std::string, Variable *);
    
    void dump(std::ofstream &);
    
    static Context *create(std::string name, Context *parents = nullptr);
};

}

#endif /* ScheatContext_h */
