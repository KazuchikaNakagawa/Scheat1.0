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
    void subscribe(void *, void(*)(void*));
    
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

void ScheatARC::subscribe(void *ptr, void(*fp)(void*) = nullptr){
    if (rmaps.find(ptr) == rmaps.end()) {
        rmaps[ptr] = ReferenceData();
        rmaps[ptr].destructor = fp;
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

bool ScheatString_isPtr(String *v){
    uint64_t t = uint64_t(v);
    std::bitset<64> bs(t);
    return bs[0] == 0;
};

String ScheatString_assign(String *v){
    if (ScheatString_isPtr(v)) {
        ScheatARC::shared().copy(v->buf.char_ptr);
    }
    return *v;
}

String ScheatString_copy(String *ss){
    String sk;
    if (ScheatString_isPtr(ss)) {
        sk.buf.char_ptr = (char *)ScheatARC::shared().copy((void *)(ss->buf.char_ptr));
    }else{
        //sk.buf.char_ptr = strcpy(sk.buf.char_ptr, ss->buf.const_chars.const_char);
        ScheatARC::shared().subscribe(sk.buf.char_ptr);
    }
    return sk;
    
}

Array Array_init(uint64_t size){
    Array arr;
    arr.begPtr = nullptr;
    arr.count = 0;
    arr.elemSize = size;
    return arr;
}

String ScheatString_add(String lhs, String rhs){
    String str;
    unsigned long ll = strlen(lhs.buf.char_ptr);
    unsigned long rl = strlen(rhs.buf.char_ptr);
    lhs.buf.char_ptr = (char *)realloc(lhs.buf.char_ptr, 8 * (ll+rl));
    
    return str;
}

void print_i32(int d){
    printf("%d", d);
};

void print_i8x(char *s){
    printf("%s", s);
};

void print_return(){
    printf("\n");
}

void print_i1(bool b){
    if (b) {
        printf("true");
    }else{
        printf("false");
    }
}

void print_String(String str){
    //printf("areare\n");
    if (ScheatString_isPtr(&str)) {
        //printf("pointer side");
        printf("%s", str.buf.char_ptr);
    }else{
        //printf("const pointer side");
        //printf("%s", str.buf.const_chars.const_char);
    }
}

void Array_append(Array *arr, void *ptr){
    arr->begPtr = realloc(arr->begPtr, arr->count * arr->elemSize + arr->elemSize);
    void *nptr = (void *)((unsigned long long)(arr->begPtr) + (arr->count * arr->elemSize));
    memcpy(nptr, ptr, arr->elemSize);
    arr->count++;
}

void *Array_at(Array *arr, int index){
    if (arr->count < index) {
        return NULL;
    }
    return (void *)((unsigned long long)(arr->begPtr) + (index * arr->elemSize));
}

String String_init(char *p){
    if (!p) {
        printf("cannot initialize String with nullpointer");
        exit(0);
    }
    //printf("source string: %s\n", p);
    String s = String();
    auto n = strlen(p);
    s.buf.char_ptr = (char *)ScheatPointer_alloc(8 * n, nullptr);
    sprintf(s.buf.char_ptr, "%s", p);
    ScheatARC::shared().subscribe(s.buf.char_ptr);
    //printf("CHARACTER POINTER WAS SET. %s\n" , s.buf.char_ptr);
    return s;
}

void *Dictionary_search(Dictionary *dict, void* key){
    for (int i = 0; i < dict->count; i++) {
        void *val = (void*)((uint64_t)dict->begPtr + dict->keySize * i);
        if (dict->compareFunc(val, key)) {
            return val;
        }
    }
    return nullptr;
}

void printn(){
    printf("\n");
}
