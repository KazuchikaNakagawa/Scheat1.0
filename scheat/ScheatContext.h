//
//  ScheatContext.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/26.
//

#ifndef ScheatContext_h
#define ScheatContext_h
#include "ScheatContextStructures.h"
#include "ScheatObjects.h"
#include <map>
#include <vector>
#include <string>
#include <stack>
namespace scheat{
using namespace std;
//using namespace basics;

class Context {
    unsigned int rnum;
    unsigned int labelcount = 0;
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
    stack<string> scopeStacks;
public:
    map<string, int> strmap = {};
    IRStream stream_entry;
    IRStream stream_body;
    IRStream stream_tail;
    std::string name;
    Context *base;
    void entryScope(string s){
        scopeStacks.push(s);
    }
    void leave(){
        scopeStacks.pop();
    }
    TypeData *type = nullptr;
    string getNowScope() {
        if (scopeStacks.empty()) return "";
        return scopeStacks.top(); };
    Variable *findVariable(std::string);
    Class *findClass(std::string);
    Function *findFunc(std::string, vector<TypeData>);
    bool isExists(std::string);
    std::string getRegister();
    Context(){
        variables = {};
        funcs = {};
        rnum = 0;
        base = nullptr;
        name = "";
        scopeStacks.push("");
    }
    
    Context(const Context &con) = default;
    
    string getLabel(){
        labelcount++;
        return "scope" + to_string(labelcount);
    }
    
    void addFunction(std::string, Function *);
    
    void addClass(std::string, Class *);
    
    void addVariable(std::string, Variable *);
    
    void dump(std::ofstream &);
    
    static Context *create(std::string name, Context *parents = nullptr);
};

class Context;

class ScheatContext {
    static stack<Context *> localcon;
    static stack<string> namespace_center;
public:
    static vector<Context *> contextCenter;
    static void Init(_Scheat *);
    static void AddMain();
    static void Shutdown(){ //delete ScheatContext::global;
        
    };
    static Context *main;
    static Context *global;
    static Context *init;
    static Context *local();
    static void pushNewNamespace(string n) { namespace_center.push(n); };
    static void popNewNamespace() { namespace_center.pop(); };
    static string getNamespace() { return namespace_center.top(); };
    static void push(Context *c) { localcon.push(c); };
    static void pop() { localcon.pop(); };
    static void printout();
    static void exportTo(ofstream &);
};

}



#endif /* ScheatContext_h */
