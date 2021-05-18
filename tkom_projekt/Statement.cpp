#include "Statement.h"


ClassDeclarationStmt::ClassDeclarationStmt(string classId)
{
	this->classId = classId;
}

FunDeclarationStmt::FunDeclarationStmt(MyType returnedType, string funId, ArgumentsStmt* argStmt)
{
	this->returnedType = returnedType;
	this->funId = funId;
	this->requiredArguments = argStmt;
}

FunCallStmt::~FunCallStmt()
{
	for (OperationElementStmt* stmt : listOfArguments)
		delete stmt;
}

BlockStmt::~BlockStmt()
{
	for (Statement* stmt : statements)
		delete stmt;
}

ArgumentsStmt::ArgumentsStmt(const vector<SignatureStmt*>& signatures)
{
	this->signatures = signatures;
}

ArgumentsStmt::~ArgumentsStmt()
{
	for (SignatureStmt* stmt : signatures)
		stmt->~SignatureStmt();
}

FunDefinitionStmt::FunDefinitionStmt(MyType returnedType, string funId, ArgumentsStmt* argStmt, BlockStmt* block)
{
	this->returnedType = returnedType;
	this->funId = funId;
	this->requiredArguments = argStmt;
	this->block = block;
}

SignatureStmt::SignatureStmt(MyType type, string id)
{
	this->type = type;
	this->id = id;
}

MyType SignatureStmt::getMyType()
{
	return this->type;
}

string SignatureStmt::getMyId()
{
	return this->id;
}

ClassSignatureStmt::ClassSignatureStmt(string className, bool isReference, string objectId)
{
	this->className = className;
	this->isReference = isReference;
	this->objectId = objectId;
}

string ClassSignatureStmt::getMyClassName()
{
	return this->className;
}
