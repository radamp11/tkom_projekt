#pragma once
#include <iostream>
#include <vector>
#include "T_Token.h"

using std::vector, std::string, std::cout, std::endl;

class Statement;
class FunDeclarationStmt;
class ClassDeclarationStmt;
class FriendClassDeclarationStmt;
class ObjectInitStmt;
class OperationElementStmt;
class ExpressionStmt;
class AddOperationStmt;
class MultOperationStmt;
class FunCallStmt;
class ObjectMethodCallStmt;
class GetObjectAttributeStmt;
class ReturnStmt;
class AssignStmt;
class InitStmt;
class SignatureStmt;
class ClassSignatureStmt;
class BlockStmt;
class ClassDefinitionStmt;
class FunDefinitionStmt;
class ArgumentsStmt;
class CallArgsStmt;

using Statement_ptr = std::unique_ptr<Statement>;
using FunDeclarationStmt_ptr = std::unique_ptr<FunDeclarationStmt>;
using ClassDeclarationStmt_ptr = std::unique_ptr<ClassDeclarationStmt>;
using FriendClassDeclarationStmt_ptr = std::unique_ptr<FriendClassDeclarationStmt>;
using ObjectInitStmt_ptr = std::unique_ptr<ObjectInitStmt>;
using OperationElementStmt_ptr = std::unique_ptr<OperationElementStmt>;
using ExpressionStmt_ptr = std::unique_ptr<ExpressionStmt>;
using AddOperationStmt_ptr = std::unique_ptr<AddOperationStmt>;
using MultOperationStmt_ptr = std::unique_ptr<MultOperationStmt>;
using FunCallStmt_ptr = std::unique_ptr<FunCallStmt>;
using ObjectMethodCallStmt_ptr = std::unique_ptr<ObjectMethodCallStmt>;
using GetObjectAttributeStmt_ptr = std::unique_ptr<GetObjectAttributeStmt>;
using ReturnStmt_ptr = std::unique_ptr<ReturnStmt>;
using AssignStmt_ptr = std::unique_ptr<AssignStmt>;
using InitStmt_ptr = std::unique_ptr<InitStmt>;
using SignatureStmt_ptr = std::unique_ptr<SignatureStmt>;
using ClassSignatureStmt_ptr = std::unique_ptr<ClassSignatureStmt>;
using BlockStmt_ptr = std::unique_ptr<BlockStmt>;
using ClassDefinitionStmt_ptr = std::unique_ptr<ClassDefinitionStmt>;
using FunDefinitionStmt_ptr = std::unique_ptr<FunDefinitionStmt>;
using ArgumentsStmt_ptr = std::unique_ptr<ArgumentsStmt>;
using CallArgsStmt_ptr = std::unique_ptr<CallArgsStmt>;


enum MyType {
	_int = 1,
	_char,
	_void,
	_constInt,
	_constChar
};


class Statement		// interface for the rest o classes
{
protected:
	int accessModifier = 0;
public:
	virtual void execute() {}
	void setAccessModifier(int accessModifier) { this->accessModifier = accessModifier; }
	int getAccessModifier() { return accessModifier; }
	virtual ~Statement() {}
};

//------------------statements definitions---------------------------


class FunDeclarationStmt : public Statement
{
public:
	FunDeclarationStmt(int accessModifier, MyType returnedType, string funId, ArgumentsStmt_ptr argStmt, bool isFriend);
	void execute() {}
	~FunDeclarationStmt() {}

	MyType getReturnedType() { return returnedType; }
	string getFunId() { return funId; }
	ArgumentsStmt_ptr& getArgumentsStmt() { return requiredArguments; }


private:
	MyType returnedType;
	bool isFriend;
	string funId;	
	ArgumentsStmt_ptr requiredArguments;
};


class ClassDeclarationStmt : public Statement
{
public:
	ClassDeclarationStmt(int accessModifier, string classId, bool isFriend);
	void execute() {}
	string getClassId() const { return classId; }

private:
	bool isFriend;
	string classId;
};


class ObjectInitStmt : public Statement
{
public:
	ObjectInitStmt(int accessModifier, string className, string objectId, FunCallStmt_ptr calledConstructor);
	void execute() {}
	~ObjectInitStmt() {}
	string getClassName() { return this->className; }
	string getObjectId() { return this->objectId; }
	FunCallStmt_ptr& getCalledConstructor() { return this->calledConstructor; }

private:
	// type of an object
	string className;
	string objectId; 
	FunCallStmt_ptr calledConstructor;
};


struct TypeAndValue {
	MyType type;
	std::variant<char, int> value;
};


class OperationElementStmt : public Statement
{
public:
	OperationElementStmt(std::variant<string, TypeAndValue, ExpressionStmt_ptr> element);
	void execute() {}
	//~OperationElementStmt();
	std::variant<string, TypeAndValue, ExpressionStmt_ptr>* getOperationElement();

private:
	//it may be:   id      int  char   other expression like fun call
	std::variant<string, TypeAndValue, ExpressionStmt_ptr> operationElement;
};


class ExpressionStmt : public Statement
{
public:
	ExpressionStmt() {}
	void execute() {}
};


class AddOperationStmt : public ExpressionStmt
{
public:
	AddOperationStmt(OperationElementStmt_ptr firstOperand, OperationElementStmt_ptr secondOperand, bool isAdd);
	void execute() {}
	~AddOperationStmt() {}
	OperationElementStmt_ptr& getFirstOperand() { return firstOperand; }
	OperationElementStmt_ptr& getSecondOperand() { return secondOperand; }

private:
	bool isAdd;		// true - add; false - sub
	OperationElementStmt_ptr firstOperand;
	OperationElementStmt_ptr secondOperand;

};

class MultOperationStmt : public ExpressionStmt
{
public:
	MultOperationStmt(OperationElementStmt_ptr firstOperand, OperationElementStmt_ptr secondOperand, bool isMult);
	void execute() {}
	~MultOperationStmt() {}
	OperationElementStmt_ptr& getFirstOperand() { return firstOperand; }
	OperationElementStmt_ptr& getSecondOperand() { return secondOperand; }

private:
	bool isMult;	// true - mult; false - div
	OperationElementStmt_ptr firstOperand;
	OperationElementStmt_ptr secondOperand;
};


class FunCallStmt : public ExpressionStmt
{
public:
	FunCallStmt(string funId, CallArgsStmt_ptr calledArgs);
	void execute() {}
	~FunCallStmt() {}
	string getFunId() { return this->funId; }
	CallArgsStmt_ptr& getCalledArgs() { return calledArgs; }

private:
	string funId;
	// every expression, so not only idintifiers, but also integers, 
	// chars, function calls, adding operations etc. might be call arguments
	CallArgsStmt_ptr calledArgs;
};


class CallArgsStmt : public Statement
{
public:
	CallArgsStmt(vector<OperationElementStmt_ptr>& args);
	void execute() {}
	~CallArgsStmt() {}
	vector<OperationElementStmt_ptr>& getListOfArguments() { return this->listOfArguments; }

private:
	vector<OperationElementStmt_ptr> listOfArguments;
};


class ObjectMethodCallStmt : public ExpressionStmt
{
public:
	ObjectMethodCallStmt(string id, FunCallStmt_ptr calledMethod);
	void execute() {}
	~ObjectMethodCallStmt() {}

private:
	string objectId;
	// operator .
	FunCallStmt_ptr calledMethod;

};


class GetObjectAttributeStmt : public ExpressionStmt
{
public:
	GetObjectAttributeStmt(string id, string attr);
	void execute() {}

private:
	string objectId;
	// operator .
	string attributeId;

};


class ReturnStmt : public Statement
{
public:
	ReturnStmt(OperationElementStmt_ptr returnedStatement);
	void execute() {}
	~ReturnStmt() {}
	OperationElementStmt_ptr& getReturnedStatement() { return this->returnedStatement; }

private:
	OperationElementStmt_ptr returnedStatement;
};


class AssignStmt : public Statement
{
public:
	AssignStmt(int accessModifier, string id, OperationElementStmt_ptr assigned);
	void execute() {}
	~AssignStmt() {}

	string getAssigneeId() { return assigneeId; }
	OperationElementStmt_ptr& getAssignedStatement() { return assignedStatement; }

private:
	string assigneeId;
	// operator =
	OperationElementStmt_ptr assignedStatement;

};


class InitStmt : public Statement
{
public:
	InitStmt(int accessModifier, MyType returnedType, string id, OperationElementStmt_ptr assignedElement);
	void execute() {}
	~InitStmt() {}
	MyType getReturnedType() { return returnedType; }
	string getId() { return id; }
	OperationElementStmt_ptr& getAssignedElement() { return assignedElement; }

private:
	MyType returnedType;
	string id;
	OperationElementStmt_ptr assignedElement;
};


class SignatureStmt : public Statement
{
public:
	SignatureStmt(MyType type, string id);
	SignatureStmt() { type = MyType::_int; id = "none"; }
	void execute() {}
	MyType getMyType();
	string getMyId();

private:
	MyType type;
	string id;

};


class ClassSignatureStmt : public SignatureStmt
{
public:
	ClassSignatureStmt(string className, bool isReference, string objectId);
	void execute() {}
	string getMyClassName();
	string getMyType() = delete;
	string getMyId() { return objectId; }
	bool getIsReference() { return isReference; }

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
	BlockStmt(vector<Statement_ptr> &statements);
	void execute() {}
	~BlockStmt() {}
	vector<Statement_ptr>& getStatements() { return statements; }

private:
	vector<Statement_ptr> statements;
};


struct InheritedClass {
	string access;
	string className;
};


class ClassDefinitionStmt : public Statement
{
public:
	ClassDefinitionStmt(int accessModifier, string className, const vector<InheritedClass>& inheritedClasses, BlockStmt_ptr classBlock);
	void execute() {}
	~ClassDefinitionStmt() {}
	string getName() { return className; }
	vector<InheritedClass>& getInheritedClasses() { return inheritedClasses; }
	BlockStmt_ptr& getDeclarationBlock() { return declarationBlock; }

private:
	string className;
	vector<InheritedClass> inheritedClasses;
	BlockStmt_ptr declarationBlock;
};


class FunDefinitionStmt : public Statement
{
public:
	FunDefinitionStmt(int accessModifier, MyType returnedType, string funId, ArgumentsStmt_ptr argStmt, BlockStmt_ptr block);
	void execute() {}
	~FunDefinitionStmt() {};
	ArgumentsStmt_ptr& getArgs() { return this->requiredArguments; }
	BlockStmt_ptr& getBlock() { return this->block; }
	bool getIsFriend() { return this->isFriend; }
	MyType getReturnedType() { return this->returnedType; }
	string getFunId() { return this->funId; }

private:
	bool isFriend;
	MyType returnedType;
	string funId;
	ArgumentsStmt_ptr requiredArguments;
	BlockStmt_ptr block;
};


class ArgumentsStmt : public Statement
{
public:
	ArgumentsStmt(vector<SignatureStmt_ptr> &signatures);
	void execute() {}
	~ArgumentsStmt();
	vector<SignatureStmt_ptr>& getSignatures() { return this->signatures; }

private:
	vector<SignatureStmt_ptr> signatures;
};

