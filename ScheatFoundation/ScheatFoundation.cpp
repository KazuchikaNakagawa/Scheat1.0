//
//  ScheatFoundation.cpp
//  Foundation
//
//  Created by かずちか on 2020/08/27.
//

#include "ScheatCFoundation.h"
#include <map>
#include <bitset>

struct ReferenceData {
    unsigned int count;
    void (*destructor)(void *);
    ReferenceData(void (*p)(void *) = nullptr){
        count = 1;
        destructor = p;
    }
};

class ScheatARC {
    std::map<void*, ReferenceData> rmaps;
    
    ScheatARC(){
        rmaps = {};
    }
public:
    
    void *create(uint64_t size, void(*)(void*));
    void *copy(void *);
    void unref(void *);
    void release(void *);
    void subscribe(void *);
    
    static ScheatARC& shared(){
        static ScheatARC arc;
        return arc;
    };
};

void *ScheatARC::create(uint64_t size, void(*fp)(void*)){
    void *ptr = malloc(size);
    
    rmaps.insert(std::make_pair(ptr, ReferenceData(fp)));
    return ptr;
}

void* ScheatARC::copy(void *ptr){
    if (rmaps.find(ptr) == rmaps.end()) {
        printf("Null Pointer Exception: copied external pointer, released pointer or undefined pointer.\n");
        exit(0);
        return nullptr;
    }
    rmaps[ptr].count++;
    return ptr;
}

void ScheatARC::unref(void *ptr){
    if (rmaps.find(ptr) == rmaps.end()) {
        printf("Null Pointer Exception: copied external pointer, released pointer or undefined pointer.\n");
        exit(0);
        return;
    }
    auto refd = &rmaps[ptr];
    int i = --(refd->count);
    if (i == 0) {
        if (refd->destructor != nullptr) {
            refd->destructor(ptr);
        }
        free(ptr);
    }
}

extern void ScheatARC::release(void *ptr){
    rmaps.erase(ptr);
    free(ptr);
}

void ScheatARC::subscribe(void *ptr){
    if (rmaps.find(ptr) == rmaps.end()) {
        rmaps[ptr] = ReferenceData();
    }else{
        rmaps[ptr].count++;
    }
}

void* ScheatPointer_alloc(uint64_t size, void(*fp)(void*)){
    return ScheatARC::shared().create(size, fp);
}

void* ScheatPointer_copy(void *ptr){
    return ScheatARC::shared().copy(ptr);
}

void ScheatPointer_unref(void *ptr){
    ScheatARC::shared().unref(ptr);
}

void ScheatPointer_release(void *ptr){
    ScheatARC::shared().release(ptr);
}

bool ScheatString_isPtr(ScheatString *v){
    uint64_t t = uint64_t(v);
    std::bitset<64> bs(t);
    return bs[0] == 0;
};

ScheatString ScheatString_assign(ScheatString *s){
    ScheatARC::shared().copy(s->buf.char_ptr);
    return *s;
}

ScheatString ScheatString_copy(ScheatString *ss){
    ScheatString sk;
    if (ScheatString_isPtr(ss)) {
        sk.buf.char_ptr = (char *)ScheatARC::shared().copy((void *)(ss->buf.char_ptr));
    }else{
        sk.buf.char_ptr = strcpy(sk.buf.char_ptr, ss->buf.const_chars.const_char);
        ScheatARC::shared().subscribe(sk.buf.char_ptr);
    }
    return sk;
    
}

ScheatString ScheatString_add(ScheatString *lhs, ScheatString *rhs){
    if (ScheatString_isPtr(lhs)) {
        
    }
    if (ScheatString_isPtr(rhs)) {
        auto k = lhs->buf.char_ptr;
        lhs->buf.char_ptr = (char *)realloc(lhs->buf.char_ptr, sizeof(lhs->buf.char_ptr) + sizeof(rhs->buf.char_ptr));
        
        sprintf(lhs->buf.char_ptr, "%s%s", k, rhs->buf.char_ptr);
        ScheatARC::shared().unref(k);
    }
    return ScheatString_copy(lhs);
}
