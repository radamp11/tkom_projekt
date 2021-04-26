#include <iostream>
#include <vector>
#include "TokenType.h"
#include "LexicalAnalyzer.h"


LexicalAnalyzer::LexicalAnalyzer(const std::string& fileName) {
	try {
		sourceFile.open(fileName);
		if (!sourceFile)
			throw "Cannot open a source file.";
	}
	catch (const std::string& eMessage) {
		std::cerr << eMessage << std::endl;
	}
	lineNumber = 1;
	currentPosition = 0;
	currentChar = getNextChar();
}

/// <summary>
/// methods to navigate through source file
/// </summary>

char LexicalAnalyzer::getNextChar() {
	currentPosition++;
	return sourceFile.get();
}


void LexicalAnalyzer::goBack() {
	currentPosition--;
	sourceFile.unget();
}
///
/// 

void LexicalAnalyzer::skipWhites() {
	while (isspace(currentChar) != 0 && currentChar != EOF) {
		if (currentChar == '\n' ) {
			lineNumber++;
			currentChar = getNextChar();
			currentPosition = 1;
		}
		else
			currentChar = getNextChar();
	}
}


bool LexicalAnalyzer::isComment() {
	if (currentChar == '/') {
		char nextChar = getNextChar();
		if (nextChar == '/') {
			currentChar = getNextChar();	// I already know it is comment, so move current char and return true
			return true;
		}
		else
			goBack();
	}
	return false;
}


void LexicalAnalyzer::skipComment() {
	if (isComment()) {
		while (currentChar != '\n' && currentChar != EOF)
			currentChar = getNextChar();
		lineNumber++;
		currentPosition = 1;
	}
}


bool LexicalAnalyzer::isLetter() { 
	return (currentChar >= 65 && currentChar <= 90 || currentChar >= 97 && currentChar <= 122); 
}


bool LexicalAnalyzer::isDigit() { 
	return currentChar >= 48 && currentChar <= 57; 
}

/// <summary>
/// returns a keyword, boolean, type or identifier token
/// </summary>

Token* LexicalAnalyzer::getIdintifierToken() {
	Token* newIdentifierToken = NULL;
	std::string tmpIdentifier = "";
	int beginningPosition = currentPosition;

	if (isLetter() || currentChar == '_') {
		do {
			tmpIdentifier += currentChar;
			currentChar = getNextChar();
		} while (isspace(currentChar) == 0 && currentChar != EOF && ( isLetter() || isDigit() || currentChar == '_'));

		if (isKeyword(tmpIdentifier))
			newIdentifierToken = buildKeyword(tmpIdentifier, beginningPosition);
		else if (isBoolean(tmpIdentifier))
			newIdentifierToken = buildBoolean(tmpIdentifier, beginningPosition);
		else if (isType(tmpIdentifier))
			newIdentifierToken = buildType(tmpIdentifier, beginningPosition);
		else
			newIdentifierToken = new Token(T_IDENTIFIER, tmpIdentifier, lineNumber, beginningPosition);
	}

	return newIdentifierToken;
}


bool LexicalAnalyzer::isKeyword(const std::string& tmp_token) {
	return tmp_token == "class" || tmp_token == "private" || tmp_token == "public" || tmp_token == "friend" || tmp_token == "return";
}


Token* LexicalAnalyzer::buildKeyword(const std::string& tmp_token, int pos) {
	Token* newKeyword = NULL;
	if (tmp_token == "return")
		newKeyword = new Token(T_KEYWORD_RETURN, tmp_token, lineNumber, pos);
	else if (tmp_token == "class")
		newKeyword = new Token(T_KEYWORD_CLASS, tmp_token, lineNumber, pos);
	else if (tmp_token == "friend")
		newKeyword = new Token(T_KEYWORD_FRIEND, tmp_token, lineNumber, pos);
	else if (tmp_token == "public")
		newKeyword = new Token(T_KEYWORD_PUBLIC, tmp_token, lineNumber, pos);
	else if (tmp_token == "private")
		newKeyword = new Token(T_KEYWORD_PRIVATE, tmp_token, lineNumber, pos);
	return newKeyword;
}


bool LexicalAnalyzer::isType(const std::string& tmp_token) {
	return (tmp_token == "int" || tmp_token == "bool" || tmp_token == "char" || tmp_token == "void");
}


Token* LexicalAnalyzer::buildType(const std::string& tmp_token, int pos) {
	Token* newTypeToken = NULL;
	if (tmp_token == "int")
		newTypeToken = new Token(T_TYPE_INT, tmp_token, lineNumber, pos);
	else if (tmp_token == "char")
		newTypeToken = new Token(T_TYPE_CHAR, tmp_token, lineNumber, pos);
	else if (tmp_token == "bool")
		newTypeToken = new Token(T_TYPE_BOOL, tmp_token, lineNumber, pos);
	else if (tmp_token == "void")
		newTypeToken = new Token(T_TYPE_VOID, tmp_token, lineNumber, pos);
	return newTypeToken;
}


bool LexicalAnalyzer::isBoolean(const std::string& tmp_token) {
	return (tmp_token == "true" || tmp_token == "false");
}


Token* LexicalAnalyzer::buildBoolean(const std::string& tmp_token, int pos) {
	if (tmp_token == "true")
		return new Token(T_VAL_TRUE, tmp_token, lineNumber, pos);
	return new Token(T_VAL_FALSE, tmp_token, lineNumber, pos);
}


/// <summary>
/// similar to getIdintifier() method, but it handles single characters tokens
/// </summary>

Token* LexicalAnalyzer::getSingleCharToken() {
	Token* newSingleCharToken = buildBoolOperator(currentChar);
	if (!newSingleCharToken)
		newSingleCharToken = buildOperator(currentChar);
	if (!newSingleCharToken)
		newSingleCharToken = buildBracket(currentChar);

	currentChar = getNextChar();

	return newSingleCharToken;
}


Token* LexicalAnalyzer::buildBoolOperator(const char& tmp_token) {
	int beginningPosition = currentPosition;
	Token* newBoolOperatorToken = NULL;
	switch (tmp_token) {
		case '<':
			newBoolOperatorToken = new Token(T_BOOL_OPERATOR_LESS, tmp_token, lineNumber, beginningPosition);
			break;

		case '>':
			newBoolOperatorToken = new Token(T_BOOL_OPERATOR_GR, tmp_token, lineNumber, beginningPosition);
			break;

		case '=': {
			char tmpChar = getNextChar();
			if (tmpChar == '=')
				newBoolOperatorToken = new Token(T_BOOL_OPERATOR_EQ, "==", lineNumber, beginningPosition);
			else
				goBack();
			break;
		}
	}
	return newBoolOperatorToken;
}


Token* LexicalAnalyzer::buildOperator(const char& tmp_token) {
	Token* newOperatorToken = NULL;
	switch (tmp_token) {
		case '/':		// it won't interfere with comments, because it is already verified and comments are skipped
			newOperatorToken = new Token(T_OPERATOR_DIV, tmp_token, lineNumber, currentPosition);
			break;

		case '*':
			newOperatorToken = new Token(T_OPERATOR_MUL, tmp_token, lineNumber, currentPosition);
			break;

		case '+':
			newOperatorToken = new Token(T_OPERATOR_ADD, tmp_token, lineNumber, currentPosition);
			break;

		case '-':
			newOperatorToken = new Token(T_OPERATOR_SUB, tmp_token, lineNumber, currentPosition);
			break;

		case '=':
			newOperatorToken = new Token(T_OPERATOR_EQ, tmp_token, lineNumber, currentPosition);
			break;

		case '.':
			newOperatorToken = new Token(T_OPERATOR_CLASS_REF, tmp_token, lineNumber, currentPosition);
			break;

		case '&':
			newOperatorToken = new Token(T_OPERATOR_REFERENCE, tmp_token, lineNumber, currentPosition);
			break;

		case '\'':
			newOperatorToken = new Token(T_OPERATOR_SINGLE_QUOTE, tmp_token, lineNumber, currentPosition);
			nextIsChar = true;
			break;

		case ':':
			newOperatorToken = new Token(T_OPERATOR_COLON, tmp_token, lineNumber, currentPosition);
			break;

		case ';':
			newOperatorToken = new Token(T_OPERATOR_SEMICOLON, tmp_token, lineNumber, currentPosition);
			break;
		}
	return newOperatorToken;
}


Token* LexicalAnalyzer::buildBracket(const char& tmp_token) {
	Token* newBracketToken = NULL;
	switch (tmp_token) {
		case '{':
			newBracketToken = new Token(T_OPEN_BLOCK_BRACKET, tmp_token, lineNumber, currentPosition);
			break;

		case '}':
			newBracketToken = new Token(T_CLOSE_BLOCK_BRACKET, tmp_token, lineNumber, currentPosition);
			break;

		case '(': 
			newBracketToken = new Token(T_OPEN_C_BRACKET, tmp_token, lineNumber, currentPosition);
			break;

		case ')':
			newBracketToken = new Token(T_CLOSE_C_BRACKET, tmp_token, lineNumber, currentPosition);
			break;
	}
	return newBracketToken;
}


Token* LexicalAnalyzer::getCharacterToken() {
	Token* newCharacterToken = new Token(T_VAL_CHARACTER, currentChar, lineNumber, currentPosition);
	currentChar = getNextChar();
	nextIsChar = false;
	return newCharacterToken;
}


Token* LexicalAnalyzer::getIntegerToken() {
	Token* newIntegerToken = NULL;
	int beginningPosition = currentPosition;
	int newIntTokenVal = 0;
	if (isDigit()) {
		newIntTokenVal = currentChar - '0';
		currentChar = getNextChar();
		while (isDigit() && currentChar != EOF) {
			newIntTokenVal = newIntTokenVal * 10 + currentChar - '0';
			currentChar = getNextChar();
		}
	}
	newIntegerToken = new Token(T_VAL_INTEGER, newIntTokenVal, lineNumber, beginningPosition);

	return newIntegerToken;
}


Token* LexicalAnalyzer::getEndToken() {
	Token* newEndToken = NULL;
	if (currentChar == EOF)
		newEndToken = new Token(T_END, EOF, lineNumber, currentPosition);
	return newEndToken;
}


Token* LexicalAnalyzer::getUnknownToken() {
	Token* newUnknownToken = NULL;
	std::string tmpUnknownVal = "";
	do {
		tmpUnknownVal += currentChar;
		currentChar = getNextChar();
	} while (isspace(currentChar) == 0 || currentChar != EOF);

	newUnknownToken = new Token(T_UNKNOWN, tmpUnknownVal, lineNumber, currentPosition);

	return newUnknownToken;
}


Token* LexicalAnalyzer::getNextToken() {
	
	Token* newToken = getEndToken();

	if (nextIsChar && !newToken)
		newToken = getCharacterToken();

	skipWhites();
	skipComment();

	if (!newToken)
		newToken = getIdintifierToken();

	if (!newToken) {
		newToken = getSingleCharToken();
		if (newToken && std::get<char>(newToken->value) == '\'') 
			nextIsChar = true;
	}
	if (!newToken)
		newToken = getIntegerToken();
	if (!newToken)
		newToken = getUnknownToken();

	return newToken;
}