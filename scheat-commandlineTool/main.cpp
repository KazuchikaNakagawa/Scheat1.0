//
//  main.cpp
//  ProjectSpica
//
//  Created by かずちか on 2020/08/03.
//

#include <stdio.h>
#include <iostream>
#include <string>
#include "scheat.h"


using namespace std;
int main(int argc, const char *argv[]){
    // std::cout << argc << std::endl;
    
    if (argc == 1) {
        scheat::Scheat s;
        printf("\033[1mScheat Compiler(C++ Edition)\033[m\n    Scheat's main compiler.\n");
        printf("    version 1.0.%d beta\n", SV_P);
        return 0;
    }else{
        printf("version 1.0.%d beta\n", SV_P);
    }
    
    if (strcmp(argv[1], "-build") == 0) {
        std::string path;
        std::cout << "main source> ";
        std::cin >> path;
        std::string includes;
        std::cout << "header search path> ";
        std::cin >> includes;
        std::string libs;
        std::cout << "library search path> ";
        std::cin >> libs;
        std::string outputFilePath;
        std::cout << "product name> ";
        std::cin >> outputFilePath;
        std::cout << "this compiling service is unavailable yet. sorry! :)" << std::endl;
        return 0;
    }
    if (strcmp(argv[1], "-play") == 0) {
        std::cout << "Scheat 2.0.1 beta(C++ Edition)" << std::endl;
        std::cout << "[based on LLVM 10.0.0]" << std::endl;
        std::cout << "> ";
        while (true) {
            std::string code;
            std::getline(std::cin, code);
            std::cout << "this feature is not available yet." << std::endl;
            break;
        }
        return 0;
    }
    if (strcmp(argv[1], "-help") == 0) {
        std::cout <<
        "-build:" << std::endl <<
        "   build Scheat source code into .o or .a file." << std::endl <<
        "   to build .a file(static library), give .scht file to 'main source'.\n" <<
        "-play:" << std::endl <<
        "   execute Scheat like a shell.\n" <<
        "-lex:" << std::endl <<
        "   lex texts and show token kind.\n"
        "   option: -f enables you to lex file. Enter file path after -f option.";
        return 0;
    }
    
    if (strcmp(argv[1], "-lex") == 0) {
        scheat::Scheat sch = scheat::Scheat();
        
        scheat::lexer::Lexer lexer(&sch);
        if (argc == 2){
            while (true){
                std::string kv;
                lexer.clearTokens();
                std::cout << "> ";
                std::getline(std::cin, kv);
                if (kv == "\\q") {
                    
                    break;
                }
                std::string buf = std::string(kv) + "\n";
                lexer.lex(kv);
                
                lexer.getTokens()->enumerate();
            }
        }else if (argc == 3 && strcmp(argv[2], "-wdebug") == 0){
            sch.allowDeepDebug(true);
            std::string kv;
            std::cout << "> ";
            std::getline(std::cin, kv);
            lexer.lex(kv);
            lexer.getTokens()->enumerate();
        }else if (argc == 3){
            std::cout << "this option was obsoluted..." << std::endl;
            return 0;
        }/*else if (argc == 4 && (strcmp(argv[2], "-f") == 0)){
            //
            std::ifstream ifs(argv[3]);
            lexer.lex(ifs);
        }*/
        if (strcmp(argv[1], "-ll") == 0) {
            string basePath(argv[2]);
            string inFilePath = basePath + ".scheat";
            string outfilePath = basePath;
            scheat::Scheat sch;
            sch.outputFilePath = outfilePath;
            sch.sourceFile = inFilePath;
            scheat::lexer::Lexer lxr(&sch);
            ifstream ifs(sch.sourceFile);
            lxr.lex(ifs);
            scheat::parser2::parse(&sch, lxr.getTokens());
            
            scheat::encoder::LLSCEncoder::encodeLL(&sch);
            return 0;
        }
        
        return 0;
    }
    printf("Illegal command options. To show helps, try scheat -help\n");
    return 0;
}
