// tkom_projekt.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include "Parser.h"
#include "Program.h"
#include "LexicalAnalyzer.h"
#include "Statement.h"


int main()
{
    //std::vector<Token_ptr> tokens;
    std::string fileName = "exampleProgram.txt";
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>(fileName);
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program(nullptr);
    try {
        program = std::move(parser->parseProgram());
    }
    catch (const string& message) {
        std::cerr << message << std::endl;
    }
    //Token_ptr newToken = std::make_unique<Token>();
    std::cout << "Koniec" << std::endl;

}