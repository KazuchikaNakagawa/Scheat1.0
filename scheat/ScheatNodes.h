//
//  ScheatNodes.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/26.
//

#ifndef ScheatNodes_h
#define ScheatNodes_h

namespace scheat {

namespace node{

class NodeData {
    
public:
    std::string value;
    std::string size;
    NodeData(std::string a, std::string b) : value(a), size(b) {};
};

}

}

#endif /* ScheatNodes_h */
