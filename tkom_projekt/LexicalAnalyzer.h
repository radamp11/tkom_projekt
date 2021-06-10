#pragma once
#include "T_Token.h"
#include <fstream>
#include <iostream>

using Token_ptr = std::unique_ptr<Token>;

class LexicalAnalyzer {
private:
	// attributes

	std::fstream sourceFile;
	int lineNumber, currentPosition;
	char currentChar;
	bool nextIsChar = false;
	bool nextIsClosingSingleQuote = false;
	bool nextIsNegativeIngeter = false;

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
	Token_ptr buildKeyword(const std::string& tmp_token, int beginningPosition);
	Token_ptr buildBoolean(const std::string& tmp_token, int beginningPosition);
	Token_ptr buildType(const std::string& tmp_token, int beginningPosition);

	Token_ptr getIdintifierToken();

	Token_ptr buildBoolOperator(const char& tmp_token);
	Token_ptr buildOperator(const char& tmp_token);
	Token_ptr buildBracket(const char& tmp_token);
	Token_ptr buildComma(const char& tmp_token);

	Token_ptr getSingleCharToken();

	Token_ptr getCharacterToken();
	Token_ptr getIntegerToken();
	Token_ptr getUnknownToken();
	Token_ptr getEndToken();

public:
	LexicalAnalyzer(const std::string& fileName);

	Token_ptr getNextToken();

	~LexicalAnalyzer() { sourceFile.close(); }
};

using LexicalAnalyzer_ptr = std::unique_ptr<LexicalAnalyzer>;
