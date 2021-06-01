#include <iostream>
#include <vector>
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
		while (currentChar != '\n' && currentChar != EOF) {
			currentChar = getNextChar();
		}
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

Token_ptr LexicalAnalyzer::getIdintifierToken() {
	Token_ptr newIdentifierToken(nullptr);
	std::string tmpIdentifier = "";
	int beginningPosition = currentPosition;

	if (isLetter() || currentChar == '_') {
		do {
			tmpIdentifier += currentChar;
			currentChar = getNextChar();
		} while (isspace(currentChar) == 0 && currentChar != EOF && ( isLetter() || isDigit() || currentChar == '_'));

		if (isKeyword(tmpIdentifier))
			newIdentifierToken = std::move(buildKeyword(tmpIdentifier, beginningPosition));
		else if (isBoolean(tmpIdentifier))
			newIdentifierToken = std::move(buildBoolean(tmpIdentifier, beginningPosition));
		else if (isType(tmpIdentifier))
			newIdentifierToken = std::move(buildType(tmpIdentifier, beginningPosition));
		else
			newIdentifierToken.reset(new Token(T_IDENTIFIER, tmpIdentifier, lineNumber, beginningPosition));
	}

	return newIdentifierToken;
}


bool LexicalAnalyzer::isKeyword(const std::string& tmp_token) {
	return tmp_token == "class" || tmp_token == "private" || tmp_token == "public" || tmp_token == "friend" || tmp_token == "return";
}


Token_ptr LexicalAnalyzer::buildKeyword(const std::string& tmp_token, int pos) {
	Token_ptr newKeyword(nullptr);
	if (tmp_token == "return")
		newKeyword.reset(new Token(T_KEYWORD_RETURN, tmp_token, lineNumber, pos));
	else if (tmp_token == "class")
		newKeyword.reset(new Token(T_KEYWORD_CLASS, tmp_token, lineNumber, pos));
	else if (tmp_token == "friend")
		newKeyword.reset(new Token(T_KEYWORD_FRIEND, tmp_token, lineNumber, pos));
	else if (tmp_token == "public")
		newKeyword.reset(new Token(T_KEYWORD_PUBLIC, tmp_token, lineNumber, pos));
	else if (tmp_token == "private")
		newKeyword.reset(new Token(T_KEYWORD_PRIVATE, tmp_token, lineNumber, pos));
	return newKeyword;
}


bool LexicalAnalyzer::isType(const std::string& tmp_token) {
	return (tmp_token == "int" || tmp_token == "bool" || tmp_token == "char" || tmp_token == "void");
}


Token_ptr LexicalAnalyzer::buildType(const std::string& tmp_token, int pos) {
	Token_ptr newTypeToken(nullptr);
	if (tmp_token == "int")
		newTypeToken.reset(new Token(T_TYPE_INT, tmp_token, lineNumber, pos));
	else if (tmp_token == "char")
		newTypeToken.reset(new Token(T_TYPE_CHAR, tmp_token, lineNumber, pos));
	else if (tmp_token == "bool")
		newTypeToken.reset(new Token(T_TYPE_BOOL, tmp_token, lineNumber, pos));
	else if (tmp_token == "void")
		newTypeToken.reset(new Token(T_TYPE_VOID, tmp_token, lineNumber, pos));
	return newTypeToken;
}


bool LexicalAnalyzer::isBoolean(const std::string& tmp_token) {
	return (tmp_token == "true" || tmp_token == "false");
}


Token_ptr LexicalAnalyzer::buildBoolean(const std::string& tmp_token, int pos) {
	if (tmp_token == "true")
		return std::make_unique<Token>(T_VAL_TRUE, tmp_token, lineNumber, pos);
	return std::make_unique<Token>(T_VAL_FALSE, tmp_token, lineNumber, pos);
}


/// <summary>
/// similar to getIdintifier() method, but it handles single characters tokens
/// </summary>

Token_ptr LexicalAnalyzer::getSingleCharToken() {
	Token_ptr newSingleCharToken = std::move(buildBoolOperator(currentChar));
	if (!newSingleCharToken.get())
		newSingleCharToken = std::move(buildOperator(currentChar));
	if (!newSingleCharToken.get())
		newSingleCharToken = std::move(buildBracket(currentChar));
	if (!newSingleCharToken.get())
		newSingleCharToken = std::move(buildComma(currentChar));

	if (newSingleCharToken.get())
		currentChar = getNextChar();

	return newSingleCharToken;
}


Token_ptr LexicalAnalyzer::buildBoolOperator(const char& tmp_token) {
	int beginningPosition = currentPosition;
	Token_ptr newBoolOperatorToken(nullptr);
	switch (tmp_token) {
		case '<':
			newBoolOperatorToken.reset(new Token(T_BOOL_OPERATOR_LESS, tmp_token, lineNumber, beginningPosition));
			break;

		case '>':
			newBoolOperatorToken.reset(new Token(T_BOOL_OPERATOR_GR, tmp_token, lineNumber, beginningPosition));
			break;

		case '=': {
			char tmpChar = getNextChar();
			if (tmpChar == '=')
				newBoolOperatorToken.reset(new Token(T_BOOL_OPERATOR_EQ, "==", lineNumber, beginningPosition));
			else
				goBack();
			break;
		}
	}
	return newBoolOperatorToken;
}


Token_ptr LexicalAnalyzer::buildOperator(const char& tmp_token) {
	Token_ptr newOperatorToken(nullptr);
	switch (tmp_token) {
		case '/':		// it won't interfere with comments, because it is already verified and comments are skipped
			newOperatorToken.reset(new Token(T_OPERATOR_DIV, tmp_token, lineNumber, currentPosition));
			break;

		case '*':
			newOperatorToken.reset(new Token(T_OPERATOR_MUL, tmp_token, lineNumber, currentPosition));
			break;

		case '+':
			newOperatorToken.reset(new Token(T_OPERATOR_ADD, tmp_token, lineNumber, currentPosition));
			break;

		case '-': {
			char tmp = getNextChar();
			if (tmp >= 48 && tmp <= 57) {
				nextIsNegativeIngeter = true;
				currentChar = tmp;
			}
			else {
				newOperatorToken.reset(new Token(T_OPERATOR_SUB, tmp_token, lineNumber, currentPosition));
				goBack();
			}
			break;
		}

		case '=':
			newOperatorToken.reset(new Token(T_OPERATOR_EQ, tmp_token, lineNumber, currentPosition));
			break;

		case '.':
			newOperatorToken.reset(new Token(T_OPERATOR_CLASS_REF, tmp_token, lineNumber, currentPosition));
			break;

		case '&':
			newOperatorToken.reset(new Token(T_OPERATOR_REFERENCE, tmp_token, lineNumber, currentPosition));
			break;

		case '\'': {
			newOperatorToken.reset(new Token(T_OPERATOR_SINGLE_QUOTE, tmp_token, lineNumber, currentPosition));
			if (!nextIsClosingSingleQuote && !nextIsChar)
				nextIsChar = true;
			else if (nextIsClosingSingleQuote && !nextIsChar)
				nextIsClosingSingleQuote = false;

			break;
		}

		case ':':
			newOperatorToken.reset(new Token(T_OPERATOR_COLON, tmp_token, lineNumber, currentPosition));
			break;

		case ';':
			newOperatorToken.reset(new Token(T_OPERATOR_SEMICOLON, tmp_token, lineNumber, currentPosition));
			break;
		}
	return newOperatorToken;
}


Token_ptr LexicalAnalyzer::buildBracket(const char& tmp_token) {
	Token_ptr newBracketToken(nullptr);
	switch (tmp_token) {
		case '{':
			newBracketToken.reset(new Token(T_OPEN_BLOCK_BRACKET, tmp_token, lineNumber, currentPosition));
			break;

		case '}':
			newBracketToken.reset(new Token(T_CLOSE_BLOCK_BRACKET, tmp_token, lineNumber, currentPosition));
			break;

		case '(': 
			newBracketToken.reset(new Token(T_OPEN_C_BRACKET, tmp_token, lineNumber, currentPosition));
			break;

		case ')':
			newBracketToken.reset(new Token(T_CLOSE_C_BRACKET, tmp_token, lineNumber, currentPosition));
			break;
	}
	return newBracketToken;
}

Token_ptr LexicalAnalyzer::buildComma(const char& tmp_token)
{
	Token_ptr newCommaToken(nullptr);
	if (tmp_token == ',')
		newCommaToken.reset(new Token(T_OPERATOR_COMMA, tmp_token, lineNumber, currentPosition));
	return newCommaToken;
}


Token_ptr LexicalAnalyzer::getCharacterToken() {
	Token_ptr newCharacterToken = std::make_unique<Token>(T_VAL_CHARACTER, currentChar, lineNumber, currentPosition);
	currentChar = getNextChar();
	nextIsChar = false;
	nextIsClosingSingleQuote = true;
	return newCharacterToken;
}


Token_ptr LexicalAnalyzer::getIntegerToken() {
	int mulFactor = 1;
	if (nextIsNegativeIngeter) {
		mulFactor = -1;
		nextIsNegativeIngeter = false;
	}
	Token_ptr newIntegerToken(nullptr);
	int beginningPosition = currentPosition;
	int newIntTokenVal = 0;
	if (isDigit()) {
		newIntTokenVal = currentChar - '0';
		currentChar = getNextChar();
		while (isDigit() && currentChar != EOF) {
			newIntTokenVal = newIntTokenVal * 10 + currentChar - '0';
			currentChar = getNextChar();
		}	
		newIntegerToken.reset(new Token(T_VAL_INTEGER, newIntTokenVal * mulFactor, lineNumber, beginningPosition));
	}

	return newIntegerToken;
}


Token_ptr LexicalAnalyzer::getEndToken() {
	Token_ptr newEndToken(nullptr);
	if (currentChar == EOF)
		newEndToken.reset(new Token(T_END, currentChar, lineNumber, currentPosition));
	return newEndToken;
}


Token_ptr LexicalAnalyzer::getUnknownToken() {
	Token_ptr newUnknownToken(nullptr);
	std::string tmpUnknownVal = "";
	
	do {
		tmpUnknownVal += currentChar;
		currentChar = getNextChar();
	} while (isspace(currentChar) == 0 && currentChar != EOF);

	newUnknownToken.reset(new Token(T_UNKNOWN, tmpUnknownVal, lineNumber, currentPosition));

	return newUnknownToken;
}


Token_ptr LexicalAnalyzer::getNextToken() {

	skipWhites();
	skipComment();
	
	Token_ptr newToken = std::move(getEndToken());

	if (nextIsChar && !newToken.get())
		newToken = std::move(getCharacterToken());

	if (nextIsNegativeIngeter && !newToken.get())
		newToken = std::move(getIntegerToken());

	if (!newToken.get())
		newToken = std::move(getIdintifierToken());

	if (!newToken.get()) {
		newToken = std::move(getSingleCharToken());
		//if (newToken && std::get<char>(newToken->value) == '\'' && !nextIsClosingSingleQuote) 
		//	nextIsChar = true;
	}

	if (!newToken.get())
		newToken = std::move(getIntegerToken());
	if (!newToken.get())
		newToken = std::move(getUnknownToken());

	return newToken;
}