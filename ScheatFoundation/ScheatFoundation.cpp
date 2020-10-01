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
        sk.buf.char_ptr = strcpy(sk.buf.char_ptr, ss->buf.const_chars.const_char);
        ScheatARC::shared().subscribe(sk.buf.char_ptr);
    }
    return sk;
    
}

String ScheatString_add(String *lhs, String *rhs){
    String str;
    unsigned long ll = 0;
    unsigned long rl = 0;
    if (ScheatString_isPtr(lhs)) {
        ll = strlen(lhs->buf.char_ptr);
    }else{
        ll = strlen(lhs->buf.const_chars.const_char);
    }
    
    if (ScheatString_isPtr(rhs)) {
        rl = strlen(rhs->buf.char_ptr);
    }else{
        rl = strlen(rhs->buf.const_chars.const_char);
    }
    
    if (ll + rl < 8) {
        memset(str.buf.const_chars.const_char, 0, sizeof(str.buf.const_chars.const_char));
        str.buf.char_ptr = str.buf.const_chars.const_char;
    }else{
        str.buf.char_ptr = (char *)ScheatARC::shared().create(8 * (ll + rl + 1), nullptr);
        memset(str.buf.char_ptr, 0, sizeof(8 * (ll + rl + 1)));
    }
    
    if (ScheatString_isPtr(lhs)) {
        sprintf(str.buf.char_ptr, "%s", lhs->buf.char_ptr);
    }else{
        sprintf(str.buf.char_ptr, "%s", lhs->buf.const_chars.const_char);
    }
    
    if (ScheatString_isPtr(rhs)) {
        sprintf(str.buf.char_ptr, "%s%s", str.buf.char_ptr, rhs->buf.char_ptr);
    }else{
        sprintf(str.buf.char_ptr, "%s%s", str.buf.char_ptr, rhs->buf.const_chars.const_char);
    }
    
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

void print_String(String *str){
    if (ScheatString_isPtr(str)) {
        printf("%s", str->buf.char_ptr);
    }else{
        printf("%s", str->buf.const_chars.const_char);
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
    return (void *)((unsigned long long)(arr->begPtr) + (arr->count * arr->elemSize));
}

String String_init(char *p){
    String s = String();
    s.buf.char_ptr = p;
    return s;
}
