#pragma once
#include "Statement.h"
#include "Execution.h"
#include "SemanticAnalyzer.h"


class Program
{
public:
	Program(std::vector<Statement_ptr> &statements);
	void checkTypes(); // enables checkTypes of every statements
	//~Program() {}
	//void checkSemantic();
	Execution exeProgram;
	void run();

	std::vector<Statement_ptr>& getStatementsList();

private:
	std::vector<Statement_ptr> statements;

	string getStatementType(Statement_ptr);

};

using Program_ptr = std::unique_ptr<Program>;

