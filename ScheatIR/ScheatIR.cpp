//
//  ScheatIR.cpp
//  scheatIRBuilder
//
//  Created by かずちか on 2020/08/06.
//

#include <stdio.h>
#include "ScheatIR.hpp"
#include "ScheatIR_Hidd.hpp"

#define crash printf("CrashReporter:\ncrashed in %u\n", __LINE__); exit(0)

using namespace scheatIR;
static IRContext *nowContext;

IR *IR::last(){
    IR *copy = this;
    if (copy->next == nullptr) {
        return copy;
    }
    while (copy = copy->next, copy->next != nullptr) {
        
    }
    return copy;
}

void IR::outll(std::string o){
    std::ofstream f(o);
    if (!f) {
        crash;
    }
    f << val << std::endl;
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

void IRHolder::outll(std::string o){
    std::ofstream f;
    f.open(o.c_str());
    if (!f.is_open()) {
        printf("CrashReporter:\ncrashed in %u\n", __LINE__);
        exit(0);
    }
    IR *copy1 = entry;
    while (copy1 != nullptr) {
        copy1->outll(o);
        copy1 = copy1->next;
    }
    IR *copy2 = body;
    while (copy2 != nullptr) {
        copy2->outll(o);
        copy2 = copy2->next;
    }
    IR *copy3 = tail;
    while (copy3 != nullptr) {
        copy3->outll(o);
        copy3 = copy3->next;
    }
}

void IR_DefineVar::outll(std::string p){
    std::ofstream f(p);
    f << "%" << id << " = alloca " << type->ir_used << std::endl;
}
