#include "Statement.h"

ClassDeclarationStmt::ClassDeclarationStmt(int accessModifier, string classId, bool isFriend) {
    this->classId = classId;
    this->isFriend = isFriend;
    this->accessModifier = accessModifier;
}

FunDeclarationStmt::FunDeclarationStmt(int accessModifier, Owner owner, MyType returnedType, string funId,
    ArgumentsStmt_ptr argStmt, bool isFriend) {
    this->returnedType = returnedType;
    this->owner = owner;
    this->funId = funId;
    this->isFriend = isFriend;
    this->accessModifier = accessModifier;
    requiredArguments = move(argStmt);
}

ArgumentsStmt::ArgumentsStmt(vector<SignatureStmt_ptr>& signatures) { this->signatures = std::move(signatures); }

ArgumentsStmt::~ArgumentsStmt() {}

FunDefinitionStmt::FunDefinitionStmt(int accessModifier, Owner owner, MyType returnedType, string funId,
    ArgumentsStmt_ptr argStmt, BlockStmt_ptr block) {
    this->returnedType = returnedType;
    this->owner = owner;
    this->funId = funId;
    this->accessModifier = accessModifier;
    this->requiredArguments = std::move(argStmt);
    this->block = std::move(block);
}

SignatureStmt::SignatureStmt(MyType type, string id) {
    this->type = type;
    this->id = id;
}

MyType SignatureStmt::getMyType() { return this->type; }

string SignatureStmt::getMyId() { return this->id; }

ClassSignatureStmt::ClassSignatureStmt(string className, bool isReference, string objectId) {
    this->className = className;
    this->isReference = isReference;
    this->objectId = objectId;
}

string ClassSignatureStmt::getMyClassName() { return this->className; }

ClassDefinitionStmt::ClassDefinitionStmt(int uniqueId, int accessModifier, string className,
    const vector<InheritedClass>& inheritedClasses, BlockStmt_ptr classBlock) {
    this->uniqueId = uniqueId;
    this->className = className;
    this->accessModifier = accessModifier;
    this->inheritedClasses = inheritedClasses;
    this->declarationBlock = std::move(classBlock);
}

BlockStmt::BlockStmt(vector<Statement_ptr>& statements) { this->statements = std::move(statements); }

ObjectInitStmt::ObjectInitStmt(int accessModifier, string className, string objectId,
    FunCallStmt_ptr calledConstructor) {
    this->className = className;
    this->accessModifier = accessModifier;
    this->objectId = objectId;
    this->calledConstructor = std::move(calledConstructor);
}

InitStmt::InitStmt(int accessModifier, MyType returnedType, string id, OperationElementStmt_ptr assignedElement) {
    this->returnedType = returnedType;
    this->id = id;
    this->accessModifier = accessModifier;
    this->assignedElement = std::move(assignedElement);
}

GetObjectAttributeStmt::GetObjectAttributeStmt(string id, string attr) {
    this->objectId = id;
    this->attributeId = attr;
}

ObjectMethodCallStmt::ObjectMethodCallStmt(string id, FunCallStmt_ptr calledMethod) {
    this->objectId = id;
    this->calledMethod = std::move(calledMethod);
}

OperationElementStmt::OperationElementStmt(std::variant<string, TypeAndValue, ExpressionStmt_ptr> element) {
    this->operationElement = std::move(element);
}

std::variant<string, TypeAndValue, ExpressionStmt_ptr>* OperationElementStmt::getOperationElement() {
    return &this->operationElement;
}

AddOperationStmt::AddOperationStmt(OperationElementStmt_ptr firstOperand, OperationElementStmt_ptr secondOperand,
    bool isAdd) {
    this->firstOperand = std::move(firstOperand);
    this->secondOperand = std::move(secondOperand);
    this->isAdd = isAdd;
}

MultOperationStmt::MultOperationStmt(OperationElementStmt_ptr firstOperand, OperationElementStmt_ptr secondOperand,
    bool isMult) {
    this->firstOperand = std::move(firstOperand);
    this->secondOperand = std::move(secondOperand);
    this->isMult = isMult;
}

ReturnStmt::ReturnStmt(OperationElementStmt_ptr returnedStatement) {
    this->returnedStatement = std::move(returnedStatement);
}

AssignStmt::AssignStmt(int accessModifier, string id, OperationElementStmt_ptr assigned) {
    this->accessModifier = accessModifier;
    this->assigneeId = id;
    this->assignedStatement = std::move(assigned);
}

FunCallStmt::FunCallStmt(string funId, CallArgsStmt_ptr calledArgs) {
    this->funId = funId;
    this->calledArgs = std::move(calledArgs);
}

CallArgsStmt::CallArgsStmt(vector<OperationElementStmt_ptr>& args) { this->listOfArguments = std::move(args); }
