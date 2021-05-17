#pragma once
#include "Statement.h"
#include "Program.h"
#include "LexicalAnalyzer.h"

class Parser
{
private:
	//attributes

	Token* currentToken;
	LexicalAnalyzer* lex;
	std::vector<Statement*> tempStatements;

	//methods
	MyType setType();
	void nextToken();
	
	ObjectInitStmt* parseObjectInitStmt();

	OperationElementStmt* parseOperationElementStmt();
	
	ExpressionStmt* parseExpressionStmt();
	//----used to generate expression stmt, which is used in operation elem stmt
	AddOperationStmt* parseAddOperationStmt();
	MultOperationStmt* parseMultOperationStmt();
	FunCallStmt* parseFunCallStmt();
	ObjectMethodCallStmt* parseObjectMethodCallStmt();
	//----end of expression statements

	GetObjectAttributeStmt* parseGetObjectAttributeStmt();
	ReturnStmt* parseReturnStmt();
	AssignStmt* parseAssignStmt();
	InitStmt* parseInitStmt();

	SignatureStmt* parseSignatureStmt();
	ClassSignatureStmt* parseClassSignatureStmt();

	BlockStmt* parseBlockStmt();
	ArgumentsStmt* parseArgumentsStmt();

	//----main statements to check
	ClassDefinitionStmt* parseClassDefinitionStmt(string classId);
	//ClassDeclarationStmt* parseClassDeclarationStmt();
	FunDefinitionStmt* parseFunDefinitionStmt(MyType returnedType, string funId, ArgumentsStmt* argStmt);

	//----uses previous private parse functions and saves new stmt in tempStatements vector
	Statement* parseNewStatement();
	bool collectAllStatements();

public:
	Parser(LexicalAnalyzer* lex);
	~Parser();
	Program* parseProgram();
};

