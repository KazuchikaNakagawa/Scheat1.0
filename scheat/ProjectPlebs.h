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
// Easy Engine for Scheat

using std::unique_ptr;
using std::vector;
using namespace std;

typedef atomic_bool Bit;
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

class VPU {
public:
    
};

class Memory {
public:
    
    virtual void log(uint64_t, ostream&);
};

class Buffer : public Memory {
public:
    uint64_t size;
    Bits buffer;
    void log(uint64_t, ostream&) override;
};

class Instance : public Memory {
public:
    vector<Buffer> sizes;
    void log(uint64_t, ostream&) override;
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

class Code {
public:
    
};

class Instruction : public Code {
public:
    
};

};
#endif /* ProjectPlebs_h */
