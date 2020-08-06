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
    // _schlib
};

struct TypeData {
    std::string name;
    std::string ir_used;
};

struct GenericTypeData : public TypeData {
    std::vector<TypeData *> temp;
    using TypeData::name;
    using TypeData::ir_used;
    GenericTypeData(){
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

struct StructureTypeData : public TypeData {
    using TypeData::name;
    using TypeData::ir_used;
    std::map<Variabledata *, int> members;
    StructureTypeData() {
        name = "";
        ir_used = "";
        members = {};
    }
};

class IR {
    std::string val;
public:
    // outputs to file.
    virtual void outll(std::string);
    
    virtual void dump(std::string);
    
    // if this ir does not have the return type, it returns nullptr
    TypeData *getType();
    
    IR *next;
    IR *prev;
    
    IR(){
        next = nullptr;
        prev = nullptr;
        val = "";
    }
    
    IR *last();
    IR *first();
};

class IR_DefineVar : public IR {
    std::string id;
    TypeData *type;
public:
    IR_DefineVar(std::string, TypeData *);
    void outll(std::string) override;
    
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
    std::vector<IRHolder> irs;
    IRHolder *insertPoint;
public:
    ScheatIR(std::string);
    bool exportTo(FileType);
    bool include(std::string, FileType);
    IRHolder *getInsertPoint() const { return insertPoint; };
    void addIR(InsertOption opt, IR *ir);
};

}

#pragma GCC visibility pop
#endif /* ScheatIR_h */
