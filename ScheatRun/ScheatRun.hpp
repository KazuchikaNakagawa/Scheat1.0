//
//  ScheatRun.hpp
//  ScheatRun
//
//  Created by かずちか on 2020/08/10.
//

#ifndef ScheatRun_
#define ScheatRun_
#include <string>

/* The classes below are exported */
#pragma GCC visibility push(default)

namespace ScheatRunSystem{

enum class InstructionType : unsigned char {
    // mov instruction: instruction number 0
    // moves a immediate value to a register.
    //     length : 8bit
    //     arg : register(8bit)
    //         | <length>
    mov = 0,
    
    // load instruction: instruction number 1
    //     length : 8bit
    //     arg : register(8bit)
    //         | address(64bit)
    load = 1,
    
    // store instruction: instruction number 2
    //     length : 8bit
    //     arg : address(64bit)
    //         | register(8bit)
    store = 2,
    
    // memmov instruction: instruction number 3
    // moves a immediate value to memory
    //     length : 8bit
    //     arg : address(64bit)
    //         | <length>
    memmov = 3,
    
    // add instruction: instruction number 4
    // calcuate two register and loads first register.
    //     length : 8bit
    //     arg : register value(8bit)
    //         | register value(8bit)
    add = 4,
    
    // sub instruction: instruction number 5
    // calcuate two register and load first register.
    //     length : 8bit
    //     arg : register value(8bit)
    //         | register value(8bit)
    sub = 5,
    
    
    
};

class Instruction;

class ScheatRun
{
public:
    void HelloWorld(const char *);
    static void FatalError(unsigned int, const char *format, ...);
    static void Debug(unsigned int, const char *format, ...);
    void run(std::string);
    void runStream(Instruction *);
};

}
#pragma GCC visibility pop
#endif
