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

//numery linii - œledzenie przed lekserem, daj znak, daj pozycje
//usuwanie nadmiaru bia³ych znaków - parser potem nie potrzebuue bia³ych znaków
// w sta³ych tekstowych bia³e znaki sa istotne
// przy cudzys³owiu lekser wykrywa ¿e nie mo¿e usun¹æ tych bia³ych znaków
//listing?
//kasowanie komentarzay - wykrywanie znaku komentarza. lekser odczytuje komentarz, ale go dalej nie przepuszcza
//wskaŸnik wyprzedzaj¹cy - skanuje w przód dopóki nie odkryje ca³ego tokena - bufor na jeden znak
// konwencja czytania kolejnych tokenów - zostawiamy znacznik albo na ostatnim znaku bie¿¹cego tokena albo na pierwszym nastêpnego 
/** rozpoznajemy:
*   s³owa kluczowe - budowa identyczna jak identyikatora - budujemy identyfikator i potem trzeba sprawdziæ, czy nie jest on s³owem kluczowym
*   regexy do rozpoznawania wzorców w tekœcie - automaty skoñæzone, jak przechodziæ z opisu diagramów na automat czy coœ
*   metaznaki - esc, nawiasy, esc znaku esc itp
*    
* 
*/

