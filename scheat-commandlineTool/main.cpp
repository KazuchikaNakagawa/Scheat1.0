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
#include "CommandLineOptions.hpp"


#define OUT(t) {printf("%s\n", t);}
#define TEST(action) { action; }

using namespace std;
using namespace scheat;
using namespace commandLine;

static void compileScheat(OptionStream &options, Option *source){
    //printf("compile: make .scheat file into object file.\n");
    Scheat scheat;
    scheat.sourceFile = string(source->value[0].data.svalue);
    
    if (scheat.sourceFile.find(".scheat") != string::npos) {
        scheat.isMain = true;
    }
    
    auto header = options.getOption("-L", type_array);
    if (!header) {
        
    }else{
        for (auto s : header->value) {
            scheat.header_search_path.push_back(s.data.svalue);
        }
        //copy(scheat.header_search_path.begin(), scheat.header_search_path.end(), header->value);
    }
    
    auto outputName = options.getOption("-o", type_string);
    if (!outputName) {
        
    }else{
        
        scheat.outputFilePath = outputName->value[0].data.svalue;
        scheat.setProductName(scheat.outputFilePath);
        //printf("output file is set: %s\n", scheat.outputFilePath.c_str());
    }
    
    auto delOption = options.getOption("-ll", type_no_args);
    if (delOption) {
        scheat.deletesIRFiles = false;
    }
    
    auto asmOption = options.getOption("-c", type_no_args);
    if (asmOption) {
        scheat.onlyAssemble = true;
    }
    
    auto deepLogOption = options.getOption("-Log", type_no_args);
    if (deepLogOption) {
        scheat.logInfo(true);
        scheat.logAllInfo(true);
    }
    
    auto LTSOption = options.getOption("-enableFastLog", type_no_args);
    
    if (LTSOption) {
        scheat.logTopString = true;
    }
    
    scheat.complementSettings();
    
    scheat.ready();
    ScheatLexer::lex();
    if (scheat.hasProbrem()) {
        return;
    }
    //printf("lexing ended successfully.");
    ScheatAnalyzer::parse();
    if (scheat.hasProbrem()) {
        return;
    }
    //printf("analyzing ended sucessfully.");
    ScheatEncoder::encode();
    //printf("encoding ended.");
    return;
}

static void introduceSelf(){
    std::cout << "Scheat 1.0" << std::endl;
    std::cout << "[based on LLVM 11/clang]" << std::endl;
}

static void playScheat(){
okok:
    introduceSelf();
    //std::cout << "> ";
    Scheat scheat;
    //scheat.allowDeepDebug(true);
    scheat.logInfo(true);
    scheat.isMain = true;
    while (true) {
        scheat.addMore();
        std::string code;
        cout << "> ";
        std::getline(std::cin, code);
        if (code == "\\q") {
            break;
        }
        if (cin.eof()) {
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
}

static void scheat_showHelp(){
    std::cout <<
    "-build <source file> (-o <output file>) (-L library search path) (-c):" << std::endl <<
    "   build Scheat source code into .o or .a file." << std::endl <<
    "   to build .a file(static library), give .scht file to 'main source'.\n" <<
    "-play:" << std::endl <<
    "   execute Scheat like a shell.\n" <<
    "-lex:" << std::endl <<
    "   lex texts and show token kind.\n"
    "   option: -f enables you to lex file. Enter file path after -f option."
    << endl;
}

int main(int argc, const char *argv[]){
    // std::cout << argc << std::endl;
    
    OptionStream options(argc, argv);
    options.parse();
    
    auto source = options.getOption("-build", type_string);
    if (!source && options.isIncluded("-build")) {
        printf("Illegal command options. To show helps, try scheat -help\n");
        return 0;
    }else if (source){
        compileScheat(options, source);
        return 0;
    }else{
        //OUT("no build options");
        //TEST(options.printBuffer());
    }
    
    auto play = options.isIncluded("-play");
    if (!play && options.isIncluded("-play")) {
        printf("Illegal command options. To show helps, try scheat -help\n");
        return 0;
    }else if (play){
        playScheat();
        return 0;
    }
    
    auto helpOption = options.getOption("-help", type_no_args);
    if (helpOption) {
        scheat_showHelp();
        return 0;
    }
    
    introduceSelf();
    
    cout << "if you don't know how to use, try -help" << endl;
    
    return 0;
    /*
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
        }else if (argc == 4 && (strcmp(argv[2], "-f") == 0)){
            //
            std::ifstream ifs(argv[3]);
            lexer.lex(ifs);
        }
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
*/
    return 0;
}
