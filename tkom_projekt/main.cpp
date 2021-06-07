// tkom_projekt.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include "Parser.h"
#include "Program.h"
#include "LexicalAnalyzer.h"
#include "Statement.h"
#include "Exceptions.h"


int main()
{
    std::vector<Token_ptr> tokens;
    std::string fileName = "exampleProgram2.txt";
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>(fileName);
    
    /*
    Token_ptr newToken = std::move(parserLex->getNextToken());
    while (newToken->type != T_END) {
        //cout << newToken->type << ", " << newToken->position.lineNum << endl;
        tokens.push_back(std::move(newToken));
        newToken = std::move(parserLex->getNextToken());
    }

    for (unsigned i = 0; i < tokens.size(); ++i) {
        tokens[i]->printToken();
    }
    */
    
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program(nullptr);
    try {
        program = std::move(parser->parseProgram());
        if (program)
            program->run();
    }
    catch (MyException& e) {
        e.printMessage();
    }

    //Token_ptr newToken = std::make_unique<Token>();
    
    std::cout << "Koniec" << std::endl;
    
}