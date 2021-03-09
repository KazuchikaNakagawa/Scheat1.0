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

// represents scope, locals
// LABEL:
//      code;

class Scope;

// represents like Class
class _Context {
    vector<Scope *> scopes;
    map<string, Function *> funcs;
public:
    bool exists(string key){
        return false;
    }
    void dump(ofstream&);
};

class Scope {
    unsigned int registerIndex = 0;
    unsigned int labelCount = 0;
    map<string, Variable *> variables = {};
    string name = "";
    Scope(string name){
        this->name = name;
    }
public:
    _Context *parent = nullptr;
    virtual bool breakable() const{ return false; };
    virtual bool continuable() const { return false; };
    string getRegister(){
        string v = "%r" + to_string(registerIndex);
        registerIndex++;
        return v;
    }
    virtual bool exists(string key){
        if (!parent) {
            return variables.find(key) != variables.end();
        }
        if (parent->exists(key)) {
            return true;
        }
        return variables.find(key) != variables.end();
    }
    IRStream entry;
    IRStream body;
    IRStream tail;
    
    void dump(ofstream &);
};

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
    
    Function *createFunction(TypeData *return_type,
                             string name,
                             string ir_name,
                             vector<TypeData> arguments);
    
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
