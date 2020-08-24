//
//  ScheatARC.hpp
//  ScheatARC
//
//  Created by かずちか on 2020/08/24.
//

#ifndef ScheatARC_
#define ScheatARC_

#include <vector>
#include <map>

/* The classes below are exported */
#pragma GCC visibility push(default)
namespace scheatARC{

class ObjectKey {
    unsigned int id;
    unsigned int count;
public:
    unsigned int getID() const { return id; };
    unsigned int getCount() const { return count; };
    
    inline bool operator < (const ObjectKey &rhs) const{
        return this->id < rhs.id;
    };
    
    bool operator == (ObjectKey rhs) const{
        return this->id == rhs.id;
    };
};

class Reference {
    unsigned int ref_id;
    std::vector<Reference> child_ref;
public:
    Reference();
    void reref(Reference *);
    void unref();
    
};

}

using namespace scheatARC;

class ScheatARC
{
    
public:
    std::map<ObjectKey, int> maps;
    __unavailable
    void HelloWorld(const char *);
    void add(Reference *);
    
    void setHandler(void(ObjectKey *));
    
    std::pair<Reference *, ObjectKey *> create(void *);
};



#pragma GCC visibility pop
#endif
