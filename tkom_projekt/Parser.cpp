#include "Parser.h"
#include <string>


MyType Parser::setType() {
	MyType type;
	if (currentToken->type == T_TYPE_INT)
		type = MyType::_int;
	else if (currentToken->type == T_TYPE_CHAR)
		type = MyType::_char;
	else if (currentToken->type == T_TYPE_BOOL)
		type = MyType::_bool;
	else if (currentToken->type == T_TYPE_VOID)
		type = MyType::_void;
	else
		throw "Undefined behaviour in setType function.";

	return type;
}

void Parser::nextToken()
{
	currentToken = lex->getNextToken();
	if (currentToken->type == T_UNKNOWN) {
		throw "Unknown token at line: " + std::to_string(currentToken->position.lineNum)
			+ ", pos: " + std::to_string(currentToken->position.linePos);
	}
}


Parser::Parser(LexicalAnalyzer* lex)
{
	this->lex = lex;
	nextToken();
}

Parser::~Parser()
{
	//for (Statement* stmt : tempStatements)
	//	delete stmt;
	//delete lex;
	delete currentToken;
}

Program* Parser::parseProgram()
{
	if (collectAllStatements())
		return new Program(this->tempStatements);
	return nullptr;
}


bool Parser::collectAllStatements()
{
	Statement* newStatement = nullptr;

	while (newStatement = parseNewStatement()) {
		tempStatements.push_back(newStatement);
		nextToken();
	}
	// if i have collected all statements from file that means a program
	// was written correctly considering syntax

	return currentToken->type == T_END;
}


Statement* Parser::parseNewStatement()
{
	Statement* newStatement = nullptr;
	vector<string> inheritedClasses;

	if (currentToken->type == T_END)
		return newStatement;

	// it is a class declaration or class definition
	if (currentToken->type == T_KEYWORD_CLASS) {
		nextToken();
		if (currentToken->type != T_IDENTIFIER)
			throw "Invalid class declaration: missing identifier at line: " + std::to_string(currentToken->position.lineNum)
				+ ", pos: " + std::to_string(currentToken->position.linePos);
		string classId = std::get<string>(currentToken->value);
		nextToken();
		if (currentToken->type == T_OPERATOR_SEMICOLON)
			return new ClassDeclarationStmt(classId);
		if (currentToken->type == T_OPERATOR_COLON) {	// there are classes inherited
			nextToken();
			do {
				if (currentToken->type != T_KEYWORD_PUBLIC || currentToken->type != T_KEYWORD_PRIVATE)
					throw "Missing inheritance access modifier at line: " + std::to_string(currentToken->position.lineNum)
						+ ", pos: " + std::to_string(currentToken->position.linePos);
				nextToken();
				if (currentToken->type != T_IDENTIFIER)
					throw "No class identifier provided at line: " + std::to_string(currentToken->position.lineNum)
						+ ", pos: " + std::to_string(currentToken->position.linePos);
				inheritedClasses.push_back(std::get<string>(currentToken->value));
				nextToken();
			} while (currentToken->type != T_OPEN_BLOCK_BRACKET);
			
		}
		if (currentToken->type == T_OPEN_BLOCK_BRACKET)
			newStatement = parseClassDefinitionStmt(classId, inheritedClasses);
		else
			throw "Missing ';' or '{' sign at line: " + std::to_string(currentToken->position.lineNum)
				+ ", pos: " + std::to_string(currentToken->position.linePos);
	}
	// it is a function declaration or function definition
	else if (currentToken->category == C_TYPE_TOKEN) {
		MyType returnedType = setType();
		nextToken();

		if (currentToken->type != T_IDENTIFIER)
			throw "Invalid function declaration: missing or wrong identifier at line: " + std::to_string(currentToken->position.lineNum)
				+ ", pos: " + std::to_string(currentToken->position.linePos);

		string funId = std::get<string>(currentToken->value);
		nextToken();

		if (currentToken->type == T_OPERATOR_SEMICOLON)
			throw "Declaration outside of function or class at line: " + std::to_string(currentToken->position.lineNum)
				+ ", pos: " + std::to_string(currentToken->position.linePos);

		if (currentToken->type != T_OPEN_C_BRACKET)
			throw "Undefined behaviour in function declaration at line: " + std::to_string(currentToken->position.lineNum)
				+ ", pos: " + std::to_string(currentToken->position.linePos);
		
		ArgumentsStmt* tmpArgumentsStatement = parseArgumentsStmt();
		// tokens are moved till token ")"
		nextToken();
		if (currentToken->type == T_OPERATOR_SEMICOLON)
			return new FunDeclarationStmt(returnedType, funId, tmpArgumentsStatement);
		if (currentToken->type == T_OPEN_BLOCK_BRACKET)
			newStatement = parseFunDefinitionStmt(returnedType, funId, tmpArgumentsStatement);
	}
	return newStatement;
}


SignatureStmt* Parser::parseSignatureStmt()
{
	// currently i know that first token is type
	MyType type = setType();
	nextToken();
	if (currentToken->type != T_IDENTIFIER)
		throw "Missing identifier at line: " + std::to_string(currentToken->position.lineNum)
			+ ", pos: " + std::to_string(currentToken->position.linePos);
	string sigId = std::get<string>(currentToken->value);
	return new SignatureStmt(type, sigId);
}

ClassSignatureStmt* Parser::parseClassSignatureStmt()
{
	// currently i know that first token is class identifier
	string className = std::get<string>(currentToken->value);
	nextToken();
	bool isReference = false;
	if (currentToken->type == T_OPERATOR_REFERENCE) {
		isReference = true;
		nextToken();
	}
	if (currentToken->type != T_IDENTIFIER)
		throw "Missing identifier at line: " + std::to_string(currentToken->position.lineNum)
			+ ", pos: " + std::to_string(currentToken->position.linePos);
	string objectId = std::get<string>(currentToken->value);
	return new ClassSignatureStmt(className, isReference, objectId);
}

ArgumentsStmt* Parser::parseArgumentsStmt()
{
	// current token is "("
	nextToken();
	std::vector<SignatureStmt*> signatures;
	SignatureStmt* tmpSignature = nullptr;
	while (currentToken->type != T_CLOSE_C_BRACKET) {
		// it is a normal signature
		if (currentToken->category == C_TYPE_TOKEN) {
			if (tmpSignature = parseSignatureStmt())
				signatures.push_back(tmpSignature);
		}
		// it is a class signature
		else if (currentToken->type == T_IDENTIFIER) {
			if (tmpSignature = parseClassSignatureStmt())
				signatures.push_back(tmpSignature);
		}
		else
			throw "Cannot parse signature in function arguments at line: " + std::to_string(currentToken->position.lineNum)
				+ ", pos: " + std::to_string(currentToken->position.linePos);
		// ended before ")" or ","
		nextToken();
		if (currentToken->type == T_CLOSE_C_BRACKET)
			break;
		if (currentToken->type != T_OPERATOR_COMMA)
			throw "Missing comma in arguments list at line: " + std::to_string(currentToken->position.lineNum)
				+ ", pos: " + std::to_string(currentToken->position.linePos);
		nextToken();
	}

	return new ArgumentsStmt(signatures);
}


// 4 main statements that program consists of



ClassDefinitionStmt* Parser::parseClassDefinitionStmt(string classId)
{
	return nullptr;
}


FunDefinitionStmt* Parser::parseFunDefinitionStmt(MyType returnedType, string funId, ArgumentsStmt* argStmt)
{
	return nullptr;
}


