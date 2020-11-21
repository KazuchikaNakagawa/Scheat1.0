//
//  ProjectPlebs.h
//  ProjectScheat
//
//  Created by かずちか on 2020/10/25.
//

#ifndef ProjectPlebs_h
#define ProjectPlebs_h
#include <iostream>
#include <vector>
#include <map>
#include <bitset>
#include <stack>
// Easy Engine for Scheat

using std::unique_ptr;
using std::vector;
using namespace std;

typedef bool Bit;
typedef Bit* Bits;

namespace plebs {

class VRAM {
private:
    vector<void*> release_list;
public:
    Bits allocate(size_t);
    void shutdown(){
        for (auto b : release_list) {
            free(b);
        }
        release_list = {};
    };
    VRAM(){
        release_list = {};
    }
};

//struct _Instance;
struct Address;

struct Instance {
    vector<Address> properties = {};
    uint64_t calcSize();
    void dump(ostream &);
};

union Value {
    short* i4Ptr;
    int8_t* i8Ptr;
    int16_t* i16Ptr;
    int32_t* i32Ptr;
    int64_t* i64Ptr;
    Instance instance;
    void dump(ostream &);
};

enum Size : short {
    i4 = 4,
    i16 = 16,
    i32 = 32,
    i64 = 64,
    instance = 0,
};

class Address {
public:
    Value value;
    Size size;
};

class Memory {
public:
    
    virtual void log(uint64_t, ostream&);
};

struct Instruction;

class VPU {
public:
    stack<Memory> caches;
    void execute();
};

class ScheatVM {
    ScheatVM() = default;
    ~ScheatVM() = default;
    map<uint64_t, Memory> variables;
public:
    uint64_t startAdrs;
    ScheatVM(const ScheatVM&) = delete;
    ScheatVM& operator=(const ScheatVM&) = delete;
    ScheatVM(ScheatVM&&) = delete;
    ScheatVM& operator=(ScheatVM&&) = delete;
    VRAM RAM;
    static ScheatVM& shared() {
        static ScheatVM instance;
        return instance;
    }
};

struct Instruction {
    
};

struct FindInst : Instruction {
    string identifier;
};

struct LoadInst : Instruction {
    
};

struct AccessInst : Instruction {
    uint index;
};

struct CallInst : Instruction {
    string identifier;
};

};
#endif /* ProjectPlebs_h */
