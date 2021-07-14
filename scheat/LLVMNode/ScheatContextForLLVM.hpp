//
//  ScheatContextForLLVM.hpp
//  scheat
//
//  Created by かずちか on 2021/07/14.
//

#ifndef ScheatContextForLLVM_hpp
#define ScheatContextForLLVM_hpp

#include <stdio.h>

#include <string>
#include <map>

using namespace std;

class SContext {
    static map<string, int> DICT;
public:
    static init(){
        DICT = {};
    }
    static string StringIndexKey;
    int get(string k) {
        return DICT[k];
    }
};

#endif /* ScheatContextForLLVM_hpp */
