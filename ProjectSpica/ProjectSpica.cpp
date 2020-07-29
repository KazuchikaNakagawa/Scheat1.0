//
//  ProjectSpica.cpp
//  ProjectSpica
//
//  Created by かずちか on 2020/07/29.
//

#include <iostream>
#include "ProjectSpica.hpp"
#include "ProjectSpicaPriv.hpp"

void ProjectSpica::HelloWorld(const char * s)
{
    ProjectSpicaPriv *theObj = new ProjectSpicaPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void ProjectSpicaPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

