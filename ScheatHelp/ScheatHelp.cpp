//
//  ScheatHelp.cpp
//  ScheatHelp
//
//  Created by かずちか on 2020/09/29.
//

#include "ScheatHelp.hpp"
#include <string>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

static map<string, vector<string>> databases;

void explain(char *key){
    string k(key);
    if (databases.find(k) == databases.end()) {
        printf("%s does not exists. Make sure there is no typo. If not, there is no data yet.\n", key);
        return;
    }
    
    auto data = databases[k];
    cout << data[0] << endl;
    cout << "  kind: " << data[1] << endl;
    cout << "  belongs: " << data[2] << endl;
    for (int i = 3; i <= data.size(); i++) {
        cout << data[i];
    }
    cout << endl;
    return;
}

void ScheatHelp(int argc, const char *argv[]){
    databases["Scheat"] = {"Scheat", "class", "Basics",
        "represents scheat's program. It is used like a path, so project settings should be done in this object."};
    databases["Scheat.delegate"] = {"delegate", "property", "Basics.Scheat",
        "look at ScheatDelegate later. if this property is not nullptr, Scheat calls some functions from this."};
    databases["Scheat.hasProblem()"] = {"hasProblem()", "function", "Basics.Scheat", "returns true if scheat has any errors in compiling."};
    databases["Scheat.debugSet(bool)"] = {"debugSet()", "function", "Basics.Scheat", "if true was set, it outputs Log."};

}

