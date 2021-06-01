#include "Program.h"

Program::Program(std::vector<Statement_ptr>& statements)
{
	this->statements = std::move(statements);
}

std::vector<Statement_ptr>& Program::getStatementsList()
{
	return this->statements;
}
