//
//  main.cpp
//  ProjectSpica
//
//  Created by かずちか on 2020/08/03.
//

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include "scheat.h"


using namespace std;
using namespace scheat;
int main(int argc, const char *argv[]){
    // std::cout << argc << std::endl;
    
    if (argc == 1) {
        scheat::_Scheat s;
        printf("\033[1mScheat Compiler(C++ Edition)\033[m\n    Scheat's main compiler.\n");
        printf("    version 1.0.%d beta\n", SV_P);
        return 0;
    }else{
        printf("version 1.0.%d beta\n", SV_P);
    }
    
    if (strcmp(argv[1], "-build") == 0) {
        Scheat scheat;
        std::string path;
        std::cout << "main source> ";
        std::cin >> scheat.sourceFile;
        std::string includes;
        std::cout << "header search path> ";
        std::cin >> scheat.header_search_path;
        std::string libs;
        std::cout << "library search path> ";
        std::cin >> scheat.library_search_path;
        std::string outputFilePath;
        std::cout << "output file> ";
        std::cin >> scheat.outputFilePath;
        cout << "product name> ";
        string l;
        cin >> l;
        scheat.setProductName(l);
        scheat.complementSettings();
        
        scheat.ready();
        ScheatLexer::lex();
        if (scheat.hasProbrem()) {
            return 0;
        }
        ScheatAnalyzer::parse();
        if (scheat.hasProbrem()) {
            return 0;
        }
        ScheatEncoder::encode();
        return 0;
    }
    if (strcmp(argv[1], "-play") == 0) {
        okok:
        std::cout << "Scheat 2.0.1 beta(C++ Edition)" << std::endl;
        std::cout << "[based on LLVM 10.0.0]" << std::endl;
        //std::cout << "> ";
        Scheat scheat;
        //scheat.allowDeepDebug(true);
        scheat.setDebugSetting(true);
        scheat.isMain = true;
        while (true) {
            scheat.addSome();
            std::string code;
            cout << "> ";
            std::getline(std::cin, code);
            if (code == "\\q") {
                break;
            }
            ScheatLexer::testlex(code + "\n");
            if (scheat.hasProbrem()) {
                cout << "----Scheat has been reset----" << endl;
                goto okok;
            }
            ScheatAnalyzer::parse();
            if (scheat.hasProbrem()) {
                cout << "----Scheat has been reset----" << endl;
                goto okok;
            }
            ScheatEncoder::printout();
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
    
    if (strcmp(argv[1], "-ready") == 0) {
        chdir("/usr/local/lib");
        int i = mkdir("Scheat", 0777);
        if (errno == EACCES ) printf("failed to access /usr/local/. try sudo chmod -R 775 /usr/localllib and retry.\n");
        else if (errno == EEXIST) printf("Scheat was already setup");
        if (i == 0) {
            printf("succeeded to get ready.");
        }
        return 0;
    }
    
    if (strcmp(argv[1], "-lex") == 0) {
        scheat::Scheat sch = scheat::Scheat();
        
        //scheat::ScheatLexer lexer(&sch);
        if (argc == 2){
            while (true){
                std::string kv;
                //lexer.clearTokens();
                std::cout << "> ";
                std::getline(std::cin, kv);
                if (kv == "\\q") {
                    
                    break;
                }
                std::string buf = std::string(kv) + "\n";
                sch.allowDeepDebug(true);
                sch.ready();
                scheat::ScheatLexer::testlex(buf);
                
                //lexer.getTokens()->enumerate();
            }
        }else if (argc == 3 && strcmp(argv[2], "-wdebug") == 0){
            sch.allowDeepDebug(true);
            sch.ready();
            std::string kv;
            std::cout << "> ";
            std::getline(std::cin, kv);
            scheat::ScheatLexer::testlex(kv);
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
            //scheat::ScheatLexer lxr(&sch);
            scheat::ScheatLexer::lex();
            if (sch.hasProbrem()) {
                return 0;
            }
            scheat::ScheatAnalyzer::parse();
            if (sch.hasProbrem()) {
                return 0;
            }
            scheat::ScheatEncoder::printout();
            scheat::ScheatEncoder::encode();
            return 0;
        }
        
        return 0;
    }
    printf("Illegal command options. To show helps, try scheat -help\n");
    return 0;
}
