//
//  ScheatEncoder.hpp
//  scheat
//
//  Created by かずちか on 2020/10/02.
//

#ifndef ScheatEncoder_hpp
#define ScheatEncoder_hpp

#include <stdio.h>
#include "Nodes2.h"
#include "ScheatObjects.h"

namespace scheat {

namespace encoder {

// Low Layer Scheat Compiler
// Compile scheat to LLVM IR an to object file.
class LLSCEncoder {
public:
    static void encode(_Scheat *);
    static void encodeLL(_Scheat *);
};

class SCVMEncoder {
    
};

};

}

#endif /* ScheatEncoder_hpp */
