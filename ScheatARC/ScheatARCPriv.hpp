//
//  ScheatARCPriv.hpp
//  ScheatARC
//
//  Created by かずちか on 2020/08/24.
//

/* The classes below are not exported */
#pragma GCC visibility push(hidden)

class ScheatARCPriv
{
    ScheatARCPriv() = default;
    ~ScheatARCPriv() = default;
    public:
    ScheatARCPriv(const ScheatARCPriv&) = delete;
    ScheatARCPriv(ScheatARCPriv&&) = delete;
    static ScheatARCPriv& shared(){
        static ScheatARCPriv obj;
        return obj;
    };
    void HelloWorldPriv(const char *);
};

#pragma GCC visibility pop
