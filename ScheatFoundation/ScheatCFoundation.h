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

extern "C" void print_i32(int d){
    printf("%d", d);
};

extern "C" void print_i8x(char *s){
    printf("%s", s);
};

extern "C" void print_i1(bool b){
    if (b) {
        printf("true");
    }else{
        printf("false");
    }
}

struct ScheatShortConstString {
    uint8_t flag;
    char const_char[7];
};

extern "C" struct ScheatString{
    union{
        ScheatShortConstString const_chars;
        char *char_ptr;
    } buf;
};

extern "C" bool ScheatString_isPtr(ScheatString *);

#endif /* ScheatCFoundation_h */
