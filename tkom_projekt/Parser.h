#pragma once
#include "Statement.h"
#include "Program.h"
#include "LexicalAnalyzer.h"

class Parser
{
private:
	//attributes

	Token_ptr currentToken;
	LexicalAnalyzer_ptr lex;
	std::vector<Statement_ptr> tempStatements;
	bool isInClassBlock = false;
	bool isInFunBlock = false;
	bool wasObjectAttrCall = false;

	int accessModifier = 0;		// 0 - don't care
								// 1 - public
								// -1 - private

	//methods
	MyType setType();
	void nextToken();
	
	ObjectInitStmt_ptr parseObjectInitStmt(string className);

	OperationElementStmt_ptr parseOperationElementStmt();
	
	ExpressionStmt_ptr parseExpressionStmt(string id);
	//----used to generate expression stmt, which is used in operation elem stmt
	AddOperationStmt_ptr parseAddOperationStmt(OperationElementStmt_ptr firstOperand, bool isAdd);
	MultOperationStmt_ptr parseMultOperationStmt(OperationElementStmt_ptr firstOperand, bool isMult);
	FunCallStmt_ptr parseFunCallStmt(string funId);
	ObjectMethodCallStmt_ptr parseObjectMethodCallStmt(string id, string calledMethod);
	GetObjectAttributeStmt_ptr parseGetObjectAttributeStmt();
	//----end of expression statements

	ReturnStmt_ptr parseReturnStmt();
	AssignStmt_ptr parseAssignStmt(string id);
	InitStmt_ptr parseInitStmt(MyType type, string id);

	SignatureStmt_ptr parseSignatureStmt();
	ClassSignatureStmt_ptr parseClassSignatureStmt();

	BlockStmt_ptr parseBlockStmt();
	ArgumentsStmt_ptr parseArgumentsStmt();
	CallArgsStmt_ptr parseCallArgsStmt();

	//----main statements to check
	ClassDefinitionStmt_ptr parseClassDefinitionStmt(int accessModifier, string classId, vector<InheritedClass>& inheritedClasses);
	//ClassDeclarationStmt* parseClassDeclarationStmt();
	FunDefinitionStmt_ptr parseFunDefinitionStmt(int accessModifier, MyType returnedType, string funId, ArgumentsStmt_ptr argStmt);

	//----uses previous private parse functions and saves new stmt in tempStatements vector
	Statement_ptr parseNewStatement();
	Statement_ptr parseClassStmt(bool isFriend);
	ExpressionStmt_ptr parseObjectCallingStmt(string id);
	bool collectAllStatements();

public:
	Parser(LexicalAnalyzer_ptr lex);
	~Parser();
	Program_ptr parseProgram();
};

using Parser_ptr = std::unique_ptr<Parser>;
