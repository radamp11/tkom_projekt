#pragma once
#include <iostream>
#include <vector>
#include "T_Token.h"

using std::vector, std::string, std::cout, std::endl;

//required declarations
class ArgumentsStmt;
class FunCallStmt;
class ExpressionStmt;
class SignatureStmt;


enum MyType {
	_int = 1,
	_char,
	_bool,
	_void,
	_constInt,
	_constChar
};


//program bêdzie mia³ listê tych statementów, bo mog¹ to byæ zarówno proste
//czyli np deklaracja klasy, jak i z³o¿one, czyli definicja funkcji, statementy


class Statement		// interfejs dla reszty klas
{
public:
	virtual void execute() = 0;
};

//------------------statements definitions---------------------------


class FunDeclarationStmt : public Statement
{
public:
	FunDeclarationStmt(MyType returnedType, string funId, ArgumentsStmt* argStmt);
	void execute() {}
	~FunDeclarationStmt() { delete requiredArguments; }

	MyType getReturnedType() { return returnedType; }
	string getFunId() { return funId; }

private:
	MyType returnedType;
//	int nazwaFunckji
	string funId;	
	ArgumentsStmt* requiredArguments;
};


class ClassDeclarationStmt : public Statement
{
public:
	ClassDeclarationStmt(string classId);
	void execute() {}
	string getClassId() const { return classId; }

private:
	string classId;
};

class FriendClassDeclarationStmt : public Statement
{
public:
	FriendClassDeclarationStmt();
	void execute() {}

private:
	bool isItFriendshipDeclaration;
	string classId;
};


class ObjectInitStmt : public Statement
{
public:
	ObjectInitStmt();
	void execute() {}
	~ObjectInitStmt() { delete calledConstructor; }

private:
	// type od an object
	string className;

	string objectId; // - this will be taken form \/ if od this fun call

	// calling constructor looks exactly like calling a function
	FunCallStmt* calledConstructor;

};



class OperationElementStmt : public Statement
{
public:
	OperationElementStmt();
	void execute() {}

private:
	//it may be:   id    int  char  bool  other expression like fun call
	std::variant<string, int/* MyType, ExpressionStmt*/> operationElement;
};


//	mo¿e zamiast dziedziczenia te¿ zrobiæ variant i kilka ifów w konstruktorze???

class ExpressionStmt : public Statement
{
public:
	ExpressionStmt();
	void execute() {}
};


class AddOperationStmt : public ExpressionStmt
{
public:
	AddOperationStmt();
	void execute() {}
	~AddOperationStmt() { delete firstOperand; delete secondOperand; }

private:
	OperationElementStmt* firstOperand;
	OperationElementStmt* secondOperand;

};

class MultOperationStmt : public ExpressionStmt
{
public:
	MultOperationStmt();
	void execute() {}
	~MultOperationStmt() { delete firstOperand; delete secondOperand; }

private:
	OperationElementStmt* firstOperand;
	OperationElementStmt* secondOperand;
};


class FunCallStmt : public ExpressionStmt
{
public:
	FunCallStmt();
	void execute() {}
	~FunCallStmt();

private:
	string funId;
	// every expression, so not only idintifiers, but also integers, 
	// chars, function calls, adding operations etc. might be call arguments

	vector<OperationElementStmt*> listOfArguments;
};


class ObjectMethodCallStmt : public ExpressionStmt
{
public:
	ObjectMethodCallStmt();
	void execute() {}
	~ObjectMethodCallStmt() { delete calledMethod; }

private:
	string objectId;
	// operator .
	FunCallStmt* calledMethod;

};


class GetObjectAttributeStmt : public Statement
{
public:
	GetObjectAttributeStmt();
	void execute() {}

private:
	string objectId;
	// operator .
	string attributeId;

};


class ReturnStmt : public Statement
{
public:
	ReturnStmt();
	void execute() {}
	~ReturnStmt() { delete returnedStatement; }

private:
	// return keyword
	OperationElementStmt* returnedStatement;

};


class AssignStmt : public Statement
{
public:
	AssignStmt();
	void execute() {}
	~AssignStmt() { delete assignedStatement; }

private:
	string assigneeId;
	// operator =
	OperationElementStmt* assignedStatement;

};


class InitStmt : public Statement
{
public:
	InitStmt();
	void execute() {}
	~InitStmt() { delete declaredStatement; }

private:
	SignatureStmt* declaredStatement;
};


class SignatureStmt : public Statement
{
public:
	SignatureStmt(MyType type, string id);
	SignatureStmt(){}
	void execute() {}

private:
	MyType type;
	string id;

};


class ClassSignatureStmt : public SignatureStmt
{
public:
	ClassSignatureStmt(string className, bool isReference, string objectId);
	void execute() {}

private:
	// class keyword;
	bool isReference;
	string className;
	string objectId;
};


//-----------------complex statements - consisted of other statements----------------

class BlockStmt : public Statement
{
public:
	BlockStmt();
	void execute() {}
	~BlockStmt();

private:
	vector<Statement*> statements;

};


class ClassDefinitionStmt : public Statement
{
public:
	ClassDefinitionStmt();
	void execute() {}
	~ClassDefinitionStmt() { delete declarationBlock; }

private:
	string className;

	// do okreœlenia jakie to maj¹ byæ typy
	vector<string> inheritedClasses;
	//

	BlockStmt* declarationBlock;
};


class FunDefinitionStmt : public Statement
{
public:
	FunDefinitionStmt(MyType returnedType, string funId, ArgumentsStmt* argStmt, BlockStmt* block);
	void execute() {}
	~FunDefinitionStmt() { delete requiredArguments; delete block; }

private:
	string returnedType;
	string funId;

	ArgumentsStmt* requiredArguments;
	BlockStmt* block;
};


//to chyba bedzie lista signatures, czyli albo type, id ; albo class_name, [&], id;
class ArgumentsStmt : public Statement
{
public:
	ArgumentsStmt(const vector<SignatureStmt*> &signatures);
	void execute() {}
	~ArgumentsStmt();

private:
	vector<SignatureStmt*> signatures;
};