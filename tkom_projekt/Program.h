#pragma once
#include "Statement.h"

class Program
{
public:
	Program(const std::vector<Statement*> &statements);
	void execute(); // enables execute of every statements
	~Program();

	Statement* getFirstStatement();

private:

	std::vector<Statement*> statements;

};

