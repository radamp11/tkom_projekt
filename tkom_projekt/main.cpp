// tkom_projekt.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include "LexicalAnalyzer.h"


int main()
{
    std::vector<Token*> tokens;
    std::string fileName = "source.txt";

    LexicalAnalyzer lex = LexicalAnalyzer(fileName);
    Token* newToken = lex.getNextToken();
    while (newToken->type != T_END) {
        tokens.push_back(newToken);
        newToken = lex.getNextToken();
    }
    for (Token* t : tokens) {
        t->printToken();
    }

    for (Token* t : tokens) {
        delete t;
    }
}