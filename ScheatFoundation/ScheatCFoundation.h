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

extern "C" void printn();

extern "C" void print_i1(bool b);

struct ScheatShortConstString {
    uint8_t flag;
    char const_char[7];
};

extern "C" struct String{
    union{
        ScheatShortConstString const_chars;
        char *char_ptr;
    } buf;
};

extern "C" void print_String(String *);

extern "C" bool ScheatString_isPtr(String *);

extern "C" String ScheatString_assign(String *);

extern "C" String ScheatString_copy(String *);

extern "C" String ScheatString_add(String *, String *);

extern "C" struct Array {
    int count;
    unsigned long long elemSize;
    void *begPtr;
};

extern "C" void Array_append(Array *, void *);

extern "C" void* Array_at(Array *, int);

#endif /* ScheatCFoundation_h */
