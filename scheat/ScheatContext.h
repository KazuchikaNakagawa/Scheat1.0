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

// Global Context
class Context {
protected:
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
    __deprecated
    void entryScope(string s){
        scopeStacks.push(s);
    }
    __deprecated
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
    
    virtual void addFunction(std::string, Function *);
    
    virtual void addClass(std::string, Class *);
    
    virtual void addVariable(std::string, Variable *);
    
    Function *createFunction(TypeData *return_type,
                             string name,
                             string ir_name,
                             vector<TypeData> arguments);
    
    void dump(std::ofstream &);
    
    static Context *create(std::string name, Context *parents = nullptr);
    /// create a local scope context and entry it
    Context *createLocal(string name){
        auto con = new Context(name, nullptr);
        con->name = name;
        stream_body << con;
        ScheatContext::push(con);
        con->labelcount = this->labelcount;
        con->rnum = this->rnum;
        con->base = this;
        return con;
    }
    virtual void _break(){
        
    };
};

class LocalContext : public Context{
    vector<Variable *> newVs;
public:
    void addFunction(std::string, Function *) override;
    void addClass(std::string, Class *) override;
    void addVariable(std::string n, Variable *v) override{
        newVs.push_back(v);
        Context::addVariable(n, v);
    }
    void _break() override;
};

}



#endif /* ScheatContext_h */
