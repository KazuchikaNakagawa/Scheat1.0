//
//  ScheatIR.cpp
//  scheatIRBuilder
//
//  Created by かずちか on 2020/08/06.
//

#include <stdio.h>
#include "ScheatIR.hpp"
#include "ScheatIR_Hidd.hpp"

using namespace scheatIR;

IR *IR::last(){
    IR *copy = this;
    if (copy->next == nullptr) {
        return copy;
    }
    while (copy = copy->next, copy->next != nullptr) {
        
    }
    return copy;
}

void IR::outll(std::ofstream o){
    o << val << std::endl;
};

IR *IR::first(){
    IR *copy = this;
    if (copy->prev == nullptr) {
        return copy;
    }
    while (copy = copy->prev, copy->prev != nullptr) {
        
    }
    return copy;
}

void IRHolder::addIRToBody(IR *ir){
    body->next = ir;
    ir->prev = body;
    body = body->next;
}

void IRHolder::addIRToTail(IR *ir){
    tail->next = ir;
    ir->prev = tail;
    tail = tail->next;
}
