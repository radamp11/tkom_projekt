// tkom_projekt.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include "LexicalAnalyzer.h"


int main()
{
    std::vector<Token*> tokens;
    std::string fileName = "exampleProgram.txt";

    LexicalAnalyzer lex = LexicalAnalyzer(fileName);
    Token* newToken = nullptr;
    do {
        newToken = lex.getNextToken();
        tokens.push_back(newToken);     
    } while (newToken->type != T_END);

    for (Token* t : tokens) {
        t->printToken();
    }

    for (Token* t : tokens) {
        delete t;
    }
}