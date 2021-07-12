//
//  ScheatLogManager.hpp
//  scheat
//
//  Created by かずちか on 2021/06/05.
//

#ifndef ScheatLogManager_hpp
#define ScheatLogManager_hpp

#include <stdio.h>
#include <fstream>
#include <string>

using namespace std;

class LogManager {
    static ofstream ofs();
    static string path;
public:
    static void init(string folderPath, string productName);
    static void warning(int,int,const char *, unsigned int, const char *,...);
    static void log(int,int,const char *, unsigned int, const char *,...);
    static void fatal(int, int, const char *, unsigned int, const char *,...);
};

#endif /* ScheatLogManager_hpp */
