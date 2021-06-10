#pragma once
#include "Execution.h"
#include "SemanticAnalyzer.h"
#include "Statement.h"

class Program {
public:
	Program(std::vector<Statement_ptr>& statements);

	Execution exeProgram;
	void run();

	std::vector<Statement_ptr>& getStatementsList();

private:
	std::vector<Statement_ptr> statements;
};

using Program_ptr = std::unique_ptr<Program>;
