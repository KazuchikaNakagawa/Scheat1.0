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

class ScheatRun
{
    ScheatRun(){};
public:
    
    static void FatalError(unsigned int, const char *format, ...);
    
    static void Debug(unsigned int, const char *format, ...);
    
    void run(std::string);
    
    ScheatRun(const ScheatRun&) = delete;
    
    static ScheatRun& shared(){
        static ScheatRun r;
        return r;
    }
};

}
#pragma GCC visibility pop
#endif
