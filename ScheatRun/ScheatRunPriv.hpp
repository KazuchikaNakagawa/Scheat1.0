//
//  ScheatRunPriv.hpp
//  ScheatRun
//
//  Created by かずちか on 2020/08/10.
//

/* The classes below are not exported */
#pragma GCC visibility push(hidden)

class ScheatRunPriv
{
    public:
    void HelloWorldPriv(const char *);
};

static void Debug(unsigned int line, const char *fmt, ...)
{
    printf("Debug\n source line%u : ", line);
    va_list arg;
    
    va_start(arg, fmt);
    ::vprintf(fmt, arg);
    va_end(arg);
    printf("\n");
}

static void FatalError(unsigned int line, const char *fmt, ...){
    printf("Error\n source line%u : ", line);
    va_list arg;
    
    va_start(arg, fmt);
    ::vprintf(fmt, arg);
    va_end(arg);
    printf("\n");
    exit(0);
}

#pragma GCC visibility pop
