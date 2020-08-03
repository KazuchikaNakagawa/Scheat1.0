//
//  main.cpp
//  ProjectSpica
//
//  Created by かずちか on 2020/08/03.
//

#include <stdio.h>
#include <iostream>
#include <string>

int main(int argc, const char *argv[]){
    if (argc == 1) {
        printf("Scheat Compiler++\n    Scheat's main compiler.\n");
        printf("version 0.0.0 beta\n");
        return 0;
    }
    if (strcmp(argv[1], "-build") == 0) {
        std::string path;
        std::cout << "main source>";
        std::cin >> path;
        std::string includes;
        std::cout << "header search path>";
        std::cin >> includes;
        std::string libs;
        std::cout << "library search path>";
        std::cin >> libs;
        std::string outputFilePath;
        std::cout << "product name>";
        std::cin >> outputFilePath;
        std::cout << "this compiling service is unavailable yet. sorry! :)" << std::endl;
    }
    if (strcmp(argv[1], "-play") == 0) {
        std::cout << "Scheat 2.0.0(C++ Edition)" << std::endl;
        std::cout << "[based on LLVM 10.0.0]" << std::endl;
        std::cout << ">";
        while (true) {
            std::string cod;
            std::cin >> cod;
            std::cout << "this feature is not available yet." << std::endl;
            break;
        }
    }
    if (strcmp(argv[1], "-help") == 0) {
        std::cout <<
        "-build:" << std::endl <<
        "   build Scheat source code into .o or .a file." << std::endl <<
        "   to build .o file, give .scht file to 'main source'.\n" <<
        "-play:" << std::endl <<
        "   execute Scheat as shell.\n" <<
        "";
    }
    
    return 0;
}
