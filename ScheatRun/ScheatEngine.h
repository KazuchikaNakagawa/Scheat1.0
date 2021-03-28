//
//  ScheatEngine.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/27.
//

#ifndef ScheatEngine_h
#define ScheatEngine_h

#include <stack>
#include <map>
#include <string>

using namespace std;

namespace scheatRun {

namespace engine {

enum EngineErrorCode {
    memory_error,
};

struct SourceLocation {
    int32_t line;
    int32_t column;
    string _to_string(){
        if (line < 0) {
            return "out of file";
        }else{
            return to_string(line) + "." + to_string(column);
        }
    }
    SourceLocation(int a, int b){
        line = a;
        column = b;
    }
    static SourceLocation outOfFile(){
        return SourceLocation(0,0);
    }
};

struct EngineError {
    EngineErrorCode code;
    SourceLocation location;
    string message = "no message...";
    string logError(){
        string buf = location._to_string() + " ";
        buf += to_string(code) + " " + message;
        return buf;
    };
};

class Heap {
    map<string, void *> variables;
public:
    
    void free();
};

typedef long long i64;
typedef int i32;
typedef short i16;
typedef char i8;

struct Register{
private:
    i64 *begin;
    i64 *bp;
public:
    Register(){
        begin = (i64 *)malloc(sizeof(i64));
        bp = begin;
    }
    ~Register(){
        delete begin;
        bp = nullptr;
    }
    
    void push(i64 d){
        begin = (i64 *)realloc(begin, bp-begin+8);
    };
    i64* R(){
        return bp;
    };
    i32* D(){
        return (i32 *)(bp + 4);
    }
    i16* W(){
        return (i16 *)(bp + 6);
    }
    i8* B(){
        return (i8*)(bp + 7);
    }
};

//ScheatのMiddleLevel Virtual Machine
class ScheatRunEngine {
public:
    EngineError *err;
    void* rbp;
    void* rsp;
    stack<Heap> heaps;
    
};

}

}

#endif /* ScheatEngine_h */
