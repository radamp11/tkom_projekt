#pragma once
#include "Statement.h"


class Program
{
public:
	Program(std::vector<Statement_ptr> &statements);
	void execute(); // enables execute of every statements
	//~Program() {}

	std::vector<Statement_ptr>& getStatementsList();

private:

	std::vector<Statement_ptr> statements;

};

using Program_ptr = std::unique_ptr<Program>;
