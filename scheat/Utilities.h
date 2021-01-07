//
//  Utilities.h
//  ProjectScheat
//
//  Created by かずちか on 2021/01/06.
//

#ifndef Utilities_h
#define Utilities_h

#include <iostream>
#include <string>

using namespace std;

extern string getFileName(string path);

namespace scheat {
enum Some{
    v,
    _nil
};

extern Some nil;

template <class Wrapped>
class Optional {
    Wrapped _w;
    Some state;
public:
    void operator=(Wrapped w){
        _w = w;
        state = v;
    }
    bool operator=(Optional<Wrapped> o){
        if (o.state == _nil){
            return false;
        }
    }
    void operator=(Some s){
        delete &_w;
        state = nil;
    }
    Wrapped operator!(){
        if (state == nil) {
            perror("Null exception pointer");
        }
        return _w;
    }
    Wrapped operator->(){
        if (state == nil) {
            perror("Null exception pointer");
        }
        return _w;
    }
};


}

#endif /* Utilities_h */
