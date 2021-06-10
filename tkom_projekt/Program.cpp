#include "Program.h"

Program::Program(std::vector<Statement_ptr>& statements) { 
	this->statements = std::move(statements); 
}

void Program::run() {
	exeProgram.run(statements);
	SemanticAnalyzer semAnalyzer = SemanticAnalyzer(exeProgram.getClassInfoTree());
	semAnalyzer.runAnalysis(statements);
	exeProgram.printResults();
}

std::vector<Statement_ptr>& Program::getStatementsList() { return this->statements; }
