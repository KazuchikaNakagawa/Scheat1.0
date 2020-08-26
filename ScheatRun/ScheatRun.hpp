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

namespace scheatRun{
namespace instruction{
enum class InstructionType : uint8_t {
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
    
    // div instruction: instruction number 6
    // calcuate two register and store first register
    //      length : 8bit
    //      arg : register value(8bit)
    //          | register value(8bit)
    div = 6,
    
    // fadd instruction: instruction number 7
    //      length : 8bit(less than 32 is unacceptable)
    //      arg : register value(8bit)
    //          | register value(8bit)
    fadd = 7,
    
    // fsub instruction: instruction number 8
    //      length : 8bit(less than 32 is unacceptable)
    //      arg : register value(8bit)
    //          | register value(8bit)
    fsub = 8,
    
    // fmul instruction: instruction number 9
    //      length : 8bit(less than 32 is unacceptable)
    //      arg : register value(8bit)
    //          | register value(8bit)
    fmul = 9,
    
    // fdiv instruction: instruction number 10
    //      length : 8bit(less than 32 is unacceptable)
    //      arg : register value(8bit)
    //          | register value(8bit)
    fdiv = 10,
    
    // fillzero instruction: instruction number 11
    //     length : 8bit()
    //     arg : address(64bit)
    fillzero = 11,
    
    // label instruction: instruction number 12
    label = 12,
    
    // extended instruction: instruction number x
    // calls external function in ScheatRun
    //     length : 8bit (shows how many argument it has.)
    //     arg : name(8bit*)
    extended,
};

// 8|1|8 bit
struct InstructionHead;

class Instruction;
}
class ScheatRun
{
    ScheatRun(){};
public:
    __unavailable
    void HelloWorld(const char *);
    
    static void FatalError(unsigned int, const char *format, ...);
    
    static void Debug(unsigned int, const char *format, ...);
    
    void run(std::string);
    
    void runStream(instruction::InstructionHead *);
    
    ScheatRun(const ScheatRun&) = delete;
    
    static ScheatRun& shared(){
        static ScheatRun r;
        return r;
    }
};

}
#pragma GCC visibility pop
#endif
