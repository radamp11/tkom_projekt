#include "Program.h"

Program::Program(const std::vector<Statement*> &statements)
{
	this->statements = statements;
}

Program::~Program()
{
	//for (Statement* stmt : statements)
	//	delete stmt;
}

Statement* Program::getFirstStatement()
{
	return statements.front();
}

std::vector<Statement*> Program::getStatementsList()
{
	return this->statements;
}
