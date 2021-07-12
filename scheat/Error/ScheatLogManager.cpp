//
//  ScheatLogManager.cpp
//  scheat
//
//  Created by かずちか on 2021/06/05.
//

#include "ScheatLogManager.hpp"
#include <time.h>
#include <stdarg.h>

void LogManager::init(string folderPath, string productName){
    
}

string LogManager::path = "";

ofstream LogManager::ofs(){
    return ofstream(path);
}

void LogManager::warning(int line, int column, const char *fileName, unsigned int fileLine, const char *format, ...){
    
}

void LogManager::log(int line, int column, const char *fileName, unsigned int fileLine, const char *format, ...)
{
    time_t now = time(NULL);
    auto local = localtime(&now);
    auto o = ofs();
    o << "[\033[1mlog\033[m][" << local->tm_hour << ":" << local->tm_min << ":" << local->tm_sec << "]\n";
    
    o << "\n";
    o.close();
}

void LogManager::fatal(int line, int column, const char *fileName, unsigned int fileLine, const char *format, ...){
    
}
