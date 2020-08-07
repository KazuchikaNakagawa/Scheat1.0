//
//  ScheatIR.hpp
//  ProjectSpica
//
//  Created by かずちか on 2020/08/06.
//

#ifndef ScheatIR_h
#define ScheatIR_h
#pragma GCC visibility push(default)

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

namespace scheatIR{

enum class FileType{
    _a,
    _o,
    _scht,
    _scheat,
    _schir,
    _ll,
    // _schlib
};

class TypeData {
    
public:
    std::string ir_used;
    std::string name;
    
    bool operator==(TypeData *rhs){
        return this->name == rhs->name
        && this->mangledName() == rhs->mangledName();
    }
    virtual std::string mangledName() const{ return ir_used; };
    TypeData(std::string nm){
        name = nm;
        ir_used = "%" + nm;
    }
};

class GenericTypeData : public TypeData {
    std::vector<TypeData *> temp;
public:
    using TypeData::name;
    
    GenericTypeData(std::string nm) : TypeData(nm){
        temp = {};
        name = "";
        ir_used = "";
    }
};

struct Variabledata {
    std::string name;
    std::string ir_used;
    TypeData *type;
};

class StructureTypeData : public TypeData {
public:
    using TypeData::name;
    std::map<Variabledata *, int> members;
    StructureTypeData(std::string nm) : TypeData(nm) {
        name = nm;
        ir_used = "%" + nm;
        members = {};
    }
};

class IR {
    std::string val;
    bool isInsertPoint;
public:
    // outputs to file.
    virtual void outll(std::string);
    
    virtual void dump(std::string);
    
    // if this ir does not have the return type, it returns nullptr
    TypeData *getType();
    
    IR *next;
    IR *prev;
    
    IR(std::string v){
        next = nullptr;
        prev = nullptr;
        val = v;
        isInsertPoint = false;
    }
    
    bool isPoint() const { return isInsertPoint;};
    
    IR *last();
    IR *first();
    
    std::string getValue() const{ return val; };
};

class IR_DefineVar : public IR {
    std::string id;
    TypeData *type;
public:
    IR_DefineVar(std::string, TypeData *);
    void outll(std::string) override;
    
};

class InsertPoint : public IR {
    
public:
    
};

class IRHolder {
    IR *entry;
    IR *body;
    IR *tail;
public:
    void addIRToEntry(IR *);
    void addIRToBody(IR *);
    void addIRToTail(IR *);
    void outll(std::string);
    void dump();
    IRHolder(){
        entry = nullptr;
        body = nullptr;
        tail = nullptr;
    }
};

class IRContext;

enum InsertOption {
    toEntry,
    toBody,
    toTail,
};

class ScheatIR {
    std::string path;
    IR *irs;
    InsertPoint *insertPoint;
    bool exportToMach_O();
public:
    ScheatIR(std::string);
    bool exportTo(FileType);
    bool include(std::string, FileType);
    InsertPoint *getInsertPoint(std::string);
    void addIR(InsertOption opt, IR *ir);
    void addMainIR();
};

}

#pragma GCC visibility pop
#endif /* ScheatIR_h */
