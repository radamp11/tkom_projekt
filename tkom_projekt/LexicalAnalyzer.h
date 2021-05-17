#pragma once
#include <iostream>
#include <fstream>
#include "T_Token.h"



class LexicalAnalyzer {
private:

	// attributes

	std::fstream sourceFile;
	int lineNumber, currentPosition;
	char currentChar;
	bool nextIsChar = false;

	// private methods
	
	char getNextChar();
	void goBack();
	void skipWhites();
	bool isComment();
	void skipComment();
	bool isLetter();
	bool isDigit();

	bool isKeyword(const std::string& tmp_token);
	bool isBoolean(const std::string& tmp_token);
	bool isType(const std::string& tmp_token);
	Token* buildKeyword(const std::string& tmp_token, int beginningPosition);
	Token* buildBoolean(const std::string& tmp_token, int beginningPosition);
	Token* buildType(const std::string& tmp_token, int beginningPosition);

	Token* getIdintifierToken();

	Token* buildBoolOperator(const char& tmp_token);
	Token* buildOperator(const char& tmp_token);
	Token* buildBracket(const char& tmp_token);
	Token* buildComma(const char& tmp_token);

	Token* getSingleCharToken();

	Token* getCharacterToken();
	Token* getIntegerToken();
	Token* getUnknownToken();
	Token* getEndToken();

public:
	LexicalAnalyzer(const std::string& fileName);

	Token* getNextToken();

	~LexicalAnalyzer() { sourceFile.close(); }
};