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
class _Function;

// represents the objects who has scopes
class _Context {
    vector<Scope *> scopes;
    map<string, _Function *> funcs;
public:
    virtual bool exists(string key){
        return funcs.find(key) != funcs.end();
    }
    void dump(ofstream&);
    /// name dont need to include any corons
    Scope *createScope(string name);
    _Function *createFunction(TypeData ret,string nm);
};

class Scope {
    unsigned int registerIndex = 0;
    unsigned int labelCount = 0;
    map<string, Variable *> variables = {};
protected:
    Scope(string name){
        this->name = name;
    }
public:
    string name = "";
    _Context *parent = nullptr;
    virtual bool breakable() const{ return false; };
    virtual bool continuable() const { return false; };
    string getRegister(){
        string v = "%r" + to_string(registerIndex);
        registerIndex++;
        return v;
    }
    bool exists(string key){
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
    friend class _Context;
    virtual void dump(ofstream &);
};

class _Function : public Scope {
public:
    TypeData return_type;
    string funcName = "";
    vector<TypeData> argTypes;
    string funcType();
    bool typesAdjust(vector<TypeData> types){
        if (argTypes.size() != types.size()) {
            return false;
        }
        for (int i = 0; i < argTypes.size(); i++) {
            if (argTypes[i].ir_used != types[i].ir_used) {
                return false;
            }
        }
        return true;
    };
    _Function(TypeData ret,string nm) : Scope("entry"){
        return_type = ret;
        funcName = nm;
    }
};

class Method;
class _Class : public _Context {
    unsigned int propCount = 0;
public:
    map<string, Property> properties;
    map<string, Method *> members;
    map<string, Operator *> operators;
    vector<TypeData *> bitMap;
    bool exists(string key) override;
    TypeData *type;
    
};

class Method : public _Function {
public:
    Method(_Class *cl, TypeData ret, string nm) : _Function(ret, nm){
        
    }
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
