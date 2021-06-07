#pragma once
#include "Statement.h"
#include "Execution.h"
#include "Exceptions.h"


struct GlobalElem {
	string type = "";	// class or fun
	ClassInfo myClassInfo;
	FunInfo myFunInfo;
};


class SemanticAnalyzer {
public:
	SemanticAnalyzer(vector<ClassInfo>& classesInfo);

	void runAnalysis(vector<Statement_ptr>& statements);
private:
	string setType(MyType type);

	void buildGlobalScope(vector<Statement_ptr>& statements);
	void checkScopes(vector<Statement_ptr>& statements);

	void checkFunBlock(FunDefinitionStmt* funDef);
	void checkClassBlock(ClassDefinitionStmt* classDef);

	void checkOperationElement(string expectedType, OperationElementStmt* operationElement, const vector<Element>& localScope, ClassInfo myClassScope, string whoCalled, string stmtName);
	void checkGetObjectAttribute(string expectedType, GetObjectAttributeStmt* objAttr, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled);
	void checkObjectMethodCall(string expectedType, ObjectMethodCallStmt* objMethodCall, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled);
	void checkFunCall(string expectedType, FunCallStmt* funCall, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled);
	string checkAssignStmt(AssignStmt* assignStmt, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled);
	void checkObjectInit(ObjectInitStmt* objInit, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled);
	Element checkIdentifier(string expectedType, string calledAttribute, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled, string stmtName);
	Element findAttributeInClassScope(string expectedType, string calledAttribute, ClassInfo myClassScope, string whoCalled);
	string findIdentifiersType(string identifier, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled);
	string findTypeInClassScope(string identifier, ClassInfo myClassScope);
	bool findMethodInClassScope(CallArgsStmt* funCallArgs, string expectedType, string calledAttribute, ClassInfo myClassScope, string whoCalled);
	void addArgsToLocalFunScope(FunDefinitionStmt* funDef, vector<Element>& localFunScope);

	ClassInfo getMyClassInfo(string myId);

	vector<ClassInfo> classesInfo;
	vector<FunInfo> functionsInfo;
	vector<GlobalElem> globalElements;

};