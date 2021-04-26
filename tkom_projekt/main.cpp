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

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

//numery linii - �ledzenie przed lekserem, daj znak, daj pozycje
//usuwanie nadmiaru bia�ych znak�w - parser potem nie potrzebuue bia�ych znak�w
// w sta�ych tekstowych bia�e znaki sa istotne
// przy cudzys�owiu lekser wykrywa �e nie mo�e usun�� tych bia�ych znak�w
//listing?
//kasowanie komentarzay - wykrywanie znaku komentarza. lekser odczytuje komentarz, ale go dalej nie przepuszcza
//wska�nik wyprzedzaj�cy - skanuje w prz�d dop�ki nie odkryje ca�ego tokena - bufor na jeden znak
// konwencja czytania kolejnych token�w - zostawiamy znacznik albo na ostatnim znaku bie��cego tokena albo na pierwszym nast�pnego 
/** rozpoznajemy:
*   s�owa kluczowe - budowa identyczna jak identyikatora - budujemy identyfikator i potem trzeba sprawdzi�, czy nie jest on s�owem kluczowym
*   regexy do rozpoznawania wzorc�w w tek�cie - automaty sko��zone, jak przechodzi� z opisu diagram�w na automat czy co�
*   metaznaki - esc, nawiasy, esc znaku esc itp
*    
* 
*/

