//
//  ScheatCFoundation.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/27.
//

#ifndef ScheatCFoundation_h
#define ScheatCFoundation_h

#include <iostream>

// ScheatPointer -:: Pointer
// automatically releases.
extern "C" void* ScheatPointer_alloc(uint64_t, void(*)(void*));

extern "C" void* ScheatPointer_copy(void *);

extern "C" void ScheatPointer_unref(void *);

extern "C" void ScheatPointer_release(void *);

extern "C" void print_i32(int d);

extern "C" void print_i8x(char *s);

extern "C" void print_return(void);

extern "C" void print_i1(bool b);

extern "C" void printn(void);

struct ScheatShortConstString {
    uint8_t flag;
    char const_char[7];
};

extern "C" struct String{
    union{
        
        char *char_ptr;
    } buf;
};

extern "C" void print_String(String);

extern "C" String String_init(char *);

extern "C" bool ScheatString_isPtr(String *);

extern "C" String ScheatString_assign(String *);

extern "C" String ScheatString_copy(String *);

extern "C" String ScheatString_add(String, String);

extern "C" struct Array {
    uint32_t count;
    unsigned long long elemSize;
    void *begPtr;
};

// Scheat 2.1...?
extern "C" struct Dictionary {
    uint32_t count;
    uint64_t keySize;
    uint64_t valueSize;
    void *begPtr;
    bool (*compareFunc)(void*,void*);
};

extern "C" void* Dictionary_search(Dictionary *, void*);

extern "C" Array Array_init(uint64_t size);

extern "C" void Array_append(Array *, void *);

extern "C" void* Array_at(Array *, int);

#endif /* ScheatCFoundation_h */
