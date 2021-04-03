//
//  ScheatBin.hpp
//  ScheatRun
//
//  Created by かずちか on 2021/04/03.
//

#ifndef ScheatBin_hpp
#define ScheatBin_hpp

#include <stdio.h>
#include <vector>
#include <iostream>

namespace instruction{


/*
 Scheat instruction
 |---8bit---|4bit|4bit|
 | instType |plce|plce|
 */

enum class InstructionType : uint8_t {
    // mov instruction: instruction number 0
    // moves a immediate value to a register.
    mov = 0,
    
    mov32,
    
    mov16,
    
    mov8,
    
    // load instruction: instruction number 1
    load,
    
    load32,
    
    load16,
    
    load8,
    
    // store instruction: instruction number 2
    store,
    
    store32,
    
    store16,
    
    store8,
    
    // func instruction
    func,
    
    // add instruction: instruction number 4
    // calcuate two register and loads first register.
    add,
    
    add32,
    
    add16,
    
    add8,
    
    // sub instruction: instruction number 5
    sub,
    
    sub32,
    
    sub16,
    
    sub8,
    
    // div instruction: instruction number 6
    div,
    
    div32,
    
    div16,
    
    div8,
    
    // fadd instruction: instruction number 7
    fadd,
    
    // fsub instruction: instruction number 8
    fsub,
    
    // fmul instruction: instruction number 9
    fmul,
    
    // fdiv instruction: instruction number 10
    fdiv,
    
    // fillzero instruction: instruction number 11
    fillzero,
    
    // label instruction: instruction number 12
    label,
    
    // ret instruction: number 13
    ret,
    
    // push instruction
    push,
    
    // pop instruction
    pop,
    
    // extended instruction: instruction number x
    extended,
};

enum RegisterPattern : uint8_t {
    r0 = 0,
    r1,// 0x1
    r2,// 0x2
    r3,// 0x3
    r4,// 0x4
    r5,// 0x5
    r6,// 0x6
    r7,// 0x7, rbp
    r8,// 0x8, rsp
    r9,// 0x9
    r10,// 0xa
    r11,// 0xb
    r12,// 0xc
    r13,// 0xd
    r14,// 0xe
    imm // 0xf
};

struct InstructionHead;

class Instruction{
    InstructionType inst;
    uint8_t layout;
    Instruction(InstructionType tp, RegisterPattern src, RegisterPattern dest){
        inst = tp;
        if (dest == imm) {
            perror("cannot place third argument immediate");
            exit(9);
        }
        layout = (uint8_t)(src) * 16 + (uint8_t)dest;
    }
};
}

#endif /* ScheatBin_hpp */
