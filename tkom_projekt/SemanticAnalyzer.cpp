#include "SemanticAnalyzer.h"
#include <string>

string SemanticAnalyzer::setType(MyType type)
{
	string stringType;
	if (type == _int)
		stringType = "int";
	else if (type == _char)
		stringType = "char";
	else if (type == _void)
		stringType = "void";
	return stringType;
}

SemanticAnalyzer::SemanticAnalyzer(vector<ClassInfo>& classesInfo)
{
	this->classesInfo = classesInfo;
}


void SemanticAnalyzer::runAnalysis(vector<Statement_ptr>& statements)
{
	buildGlobalScope(statements);
	checkScopes(statements);
}


void SemanticAnalyzer::buildGlobalScope(vector<Statement_ptr>& statements)
{
	string stmtName;
	for (unsigned i = 0; i < statements.size(); ++i) {
		stmtName = statements[i]->getStmtClassName();
		if (stmtName == "class ClassDefinitionStmt *") {
			ClassDefinitionStmt* classDef = dynamic_cast<ClassDefinitionStmt*>(statements[i].get());
			GlobalElem newElem;
			newElem.type = "class";
			newElem.myClassInfo = getMyClassInfo(classDef->getName());
			globalElements.push_back(newElem);
		}
		else if (stmtName == "class FunDefinitionStmt *") {
			FunDefinitionStmt* funDef = dynamic_cast<FunDefinitionStmt*>(statements[i].get());
			FunInfo funInfo;
			funInfo.id = funDef->getFunId();
			funInfo.type = setType(funDef->getReturnedType());
			ArgumentsStmt* funArgs = dynamic_cast<ArgumentsStmt*>(funDef->getArgs().get());
			for (unsigned i = 0; i < funArgs->getSignatures().size(); ++i) {
				SignatureStmt* sig = funArgs->getSignatures()[i].get();
				stmtName = sig->getStmtClassName();
				Element elem;
				if (stmtName == "class ClassSignatureStmt *") {
					ClassSignatureStmt* classSig = dynamic_cast<ClassSignatureStmt*>(sig);
					elem.type = classSig->getMyClassName();
					elem.id = classSig->getMyId();
				}
				else if (stmtName == "class SignatureStmt *") {
					elem.type = setType(sig->getMyType());
					elem.id = sig->getMyId();
				}
				funInfo.arguments.push_back(elem);
			}
			GlobalElem newElem;
			newElem.type = "function";
			newElem.myFunInfo = funInfo;
			globalElements.push_back(newElem);
		}
	}
}


void SemanticAnalyzer::checkScopes(vector<Statement_ptr>& statements)
{
	string stmtName;

	for (unsigned i = 0; i < statements.size(); ++i) {
		stmtName = statements[i]->getStmtClassName();
		if (stmtName == "class ClassDefinitionStmt *") {
			ClassDefinitionStmt* classDef = dynamic_cast<ClassDefinitionStmt*>(statements[i].get());
			checkClassBlock(classDef);
		}
		else if (stmtName == "class FunDefinitionStmt *") {
			FunDefinitionStmt* funDef = dynamic_cast<FunDefinitionStmt*>(statements[i].get());
			checkFunBlock(funDef);
		}
	}
}


void SemanticAnalyzer::checkFunBlock(FunDefinitionStmt* funDef)
{
	string stmtName;
	vector<Element> localFunScope;
	string myOwner = funDef->getOwner();
	BlockStmt* funBlock = funDef->getBlock().get();
	ClassInfo myClassScope;
	if (myOwner != "global")
		myClassScope = getMyClassInfo(myOwner);

	addArgsToLocalFunScope(funDef, localFunScope);

	for (unsigned i = 0; i < funBlock->getStatements().size(); ++i) {
		stmtName = funBlock->getStatements()[i]->getStmtClassName();
		if (stmtName == "class ClassDefinitionStmt *") {
			ClassDefinitionStmt* classDef = dynamic_cast<ClassDefinitionStmt*>(funBlock->getStatements()[i].get());
			Element newElem;
			newElem.type = classDef->getName();
			newElem.id = classDef->getName();
			localFunScope.push_back(newElem);
			checkClassBlock(classDef);
		}
		else if (stmtName == "class ClassDeclarationStmt *") {
			ClassDeclarationStmt* classDec = dynamic_cast<ClassDeclarationStmt*>(funBlock->getStatements()[i].get());
			Element newElem;
			newElem.type = classDec->getClassId();
			newElem.id = classDec->getClassId();
			localFunScope.push_back(newElem);
		}
		else if (stmtName == "class InitStmt *") {
			InitStmt* initStmt = dynamic_cast<InitStmt*>(funBlock->getStatements()[i].get());
			string expectedType = setType(initStmt->getReturnedType());
			checkOperationElement(expectedType, initStmt->getAssignedElement().get(), localFunScope, myClassScope, funDef->getFunId(), "init " + initStmt->getId());
			Element newElem;
			newElem.type = expectedType;
			newElem.id = initStmt->getId();
			localFunScope.push_back(newElem);
		}
		else if (stmtName == "class FunDefinitionStmt *" || stmtName == "class FunDeclarationStmt *") {
			throw SemanticException("Function: " + funDef->getFunId() + " - You cannot define or declare a function inside another function");
		}
		else if (stmtName == "class GetObjectAttributeStmt *") {
			GetObjectAttributeStmt* objAttr = dynamic_cast<GetObjectAttributeStmt*>(funBlock->getStatements()[i].get());
			checkGetObjectAttribute("none", objAttr, localFunScope, myClassScope, funDef->getFunId());
		}
		else if (stmtName == "class ObjectMethodCallStmt *") {
			ObjectMethodCallStmt* objMethodCall = dynamic_cast<ObjectMethodCallStmt*>(funBlock->getStatements()[i].get());
			checkObjectMethodCall("none", objMethodCall, localFunScope, myClassScope, funDef->getFunId());
		}
		else if (stmtName == "class FunCallStmt *") {
			FunCallStmt* funCall = dynamic_cast<FunCallStmt*>(funBlock->getStatements()[i].get());
			checkFunCall("none", funCall, localFunScope, myClassScope, funDef->getFunId());
		}
		else if (stmtName == "class AssignStmt *") {
			AssignStmt* assignStmt = dynamic_cast<AssignStmt*>(funBlock->getStatements()[i].get());
			string expectedType = checkAssignStmt(assignStmt, localFunScope, myClassScope, funDef->getFunId());
			OperationElementStmt* operationElement = assignStmt->getAssignedStatement().get();
			checkOperationElement(expectedType, operationElement, localFunScope, myClassScope, funDef->getFunId(), "assign " + assignStmt->getAssigneeId());
		}
		else if (stmtName == "class ObjectInitStmt *") {
			ObjectInitStmt* objInit = dynamic_cast<ObjectInitStmt*>(funBlock->getStatements()[i].get());
			checkObjectInit(objInit, localFunScope, myClassScope, funDef->getFunId());
			Element newElem;
			newElem.type = objInit->getClassName();
			newElem.id = objInit->getObjectId();
			localFunScope.push_back(newElem);
		}
		else if (stmtName == "class ReturnStmt *") {
			ReturnStmt* returnStmt = dynamic_cast<ReturnStmt*>(funBlock->getStatements()[i].get());
			string expectedType = setType(funDef->getReturnedType());
			checkOperationElement(expectedType, returnStmt->getReturnedStatement().get(), localFunScope, myClassScope, funDef->getFunId(), "return statement");
		}
	}
}

void SemanticAnalyzer::checkClassBlock(ClassDefinitionStmt* classDef)
{
	string stmtName;
	vector<Element> localScope;
	ClassInfo myClassScope = getMyClassInfo(classDef->getName());
	BlockStmt* classBlock = classDef->getDeclarationBlock().get();
	
	for (unsigned i = 0; i < classBlock->getStatements().size(); ++i) {
		stmtName = classBlock->getStatements()[i]->getStmtClassName();
		if (stmtName == "class ClassDefinitionStmt *") {
			ClassDefinitionStmt* classDef = dynamic_cast<ClassDefinitionStmt*>(classBlock->getStatements()[i].get());
			Element newElem;
			newElem.type = classDef->getName();
			newElem.id = classDef->getName();
			localScope.push_back(newElem);
			checkScopes(classBlock->getStatements());
		}
		else if (stmtName == "class ClassDeclarationStmt *") {
			ClassDeclarationStmt* classDec = dynamic_cast<ClassDeclarationStmt*>(classBlock->getStatements()[i].get());
			Element newElem;
			newElem.type = classDec->getClassId();
			newElem.id = classDec->getClassId();
			localScope.push_back(newElem);
		}
		else if (stmtName == "class InitStmt *") {
			InitStmt* initStmt = dynamic_cast<InitStmt*>(classBlock->getStatements()[i].get());
			string expectedType = setType(initStmt->getReturnedType());
			OperationElementStmt* assignedElem = initStmt->getAssignedElement().get();
			if(assignedElem)
				checkOperationElement(expectedType, assignedElem, localScope, myClassScope, classDef->getName(), "init " + initStmt->getId());
			Element newElem;
			newElem.type = expectedType;
			newElem.id = initStmt->getId();
			localScope.push_back(newElem);
		}
		else if (stmtName == "class FunDefinitionStmt *") {
			FunDefinitionStmt* funDef = dynamic_cast<FunDefinitionStmt*>(classBlock->getStatements()[i].get());
			checkFunBlock(funDef);
		}
		else if (stmtName == "class GetObjectAttributeStmt *") {
			GetObjectAttributeStmt* objAttr = dynamic_cast<GetObjectAttributeStmt*>(classBlock->getStatements()[i].get());
			throw SemanticException("Unallowed statement in class " + classDef->getName() + ": you cannot refer direclty to object's attribute: " +
				objAttr->getObjectId() + "." + objAttr->getAttributeId() + " in class declaration block.");
		}
		else if (stmtName == "class ObjectMethodCallStmt *") {
			ObjectMethodCallStmt* methodCall = dynamic_cast<ObjectMethodCallStmt*>(classBlock->getStatements()[i].get());
			throw SemanticException("Unallowed statement in class " + classDef->getName() + ": you cannot refer direclty to object's method: " +
				methodCall->getObjectId() + "." + methodCall->getCalledMethod()->getFunId() + " in class declaration block.");
		}
		else if (stmtName == "class FunCallStmt *") {
			FunCallStmt* funCall = dynamic_cast<FunCallStmt*>(classBlock->getStatements()[i].get());
			throw SemanticException("Unallowed statement in class " + classDef->getName() + ": you cannot refer directly to function call: "
				+ funCall->getFunId() + " in class declaration block.");
		}
		else if (stmtName == "class AssignStmt *") {
			AssignStmt* assignStmt = dynamic_cast<AssignStmt*>(classBlock->getStatements()[i].get());
			throw SemanticException("Unallowed statement in class " + classDef->getName() + ": assignment: "+ assignStmt->getAssigneeId() +
				" in class declaration block. Did you mean to init new class member?");
		}
		else if (stmtName == "class ObjectInitStmt *") {
			ObjectInitStmt* objInit = dynamic_cast<ObjectInitStmt*>(classBlock->getStatements()[i].get());
			checkObjectInit(objInit, localScope, myClassScope, classDef->getName());
			Element newElem;
			newElem.type = objInit->getClassName();
			newElem.id = objInit->getObjectId();
			localScope.push_back(newElem);
		}
		else if (stmtName == "class ReturnStmt *") {
			ReturnStmt* returnStmt = dynamic_cast<ReturnStmt*>(classBlock->getStatements()[i].get());
			throw SemanticException("Unallowed return statement in class " + classDef->getName() + "- you cannot return anything in class declaration block.");
		}
	}
}

void SemanticAnalyzer::checkOperationElement(string expectedType, OperationElementStmt* operationElement, const vector<Element>& localScope, ClassInfo myClassScope, string whoCalled, string stmtName)
{
	string* calledAttribute = std::get_if<string>(operationElement->getOperationElement());
	if (calledAttribute) {	// it is an attribute called
		checkIdentifier(expectedType, *calledAttribute, localScope, myClassScope, whoCalled, stmtName);
	}
	TypeAndValue* calledValue = std::get_if<TypeAndValue>(operationElement->getOperationElement());
	if (calledValue) {
		string type = setType(calledValue->type);
		if (expectedType != type) {
			string message;
			if (type == "char") {
				message = "char ";
				message += std::get<char>(calledValue->value);
			}
			else if (type == "int")
				message = "int " + std::to_string(std::get<int>(calledValue->value));
			throw SemanticException("Invalid value in: " + stmtName + " in function/class: " + whoCalled + ". Invalid type of " + message + " element.");
		}
	}
	else {	// it is an expression
		auto exprPtr = std::get_if<ExpressionStmt_ptr>(operationElement->getOperationElement());
		if (exprPtr == nullptr)
			return;
		ExpressionStmt* expression = exprPtr->get();
		// checking kind of expression
		FunCallStmt* funCall = dynamic_cast<FunCallStmt*>(expression);
		GetObjectAttributeStmt* objAttr = dynamic_cast<GetObjectAttributeStmt*>(expression);
		ObjectMethodCallStmt* objMethodCall = dynamic_cast<ObjectMethodCallStmt*>(expression);
		AddOperationStmt* addOper = dynamic_cast<AddOperationStmt*>(expression);
		MultOperationStmt* multOper = dynamic_cast<MultOperationStmt*>(expression);
		// if it is not one of expressions above, dynamic_cast returns nullptr
		if (funCall) {
			checkFunCall(expectedType, funCall, localScope, myClassScope, whoCalled);
		}
		else if (objAttr) {
			checkGetObjectAttribute(expectedType, objAttr, localScope, myClassScope, whoCalled);
		}
		else if (objMethodCall) {
			checkObjectMethodCall(expectedType, objMethodCall, localScope, myClassScope, whoCalled);
		}
		else if (addOper) {
			OperationElementStmt* firstOperand = addOper->getFirstOperand().get();
			OperationElementStmt* secondOperand = addOper->getSecondOperand().get();
			checkOperationElement(expectedType, firstOperand, localScope, myClassScope, whoCalled, stmtName);
			checkOperationElement(expectedType, secondOperand, localScope, myClassScope, whoCalled, stmtName);
		}
		else if (multOper) {
			OperationElementStmt* firstOperand = multOper->getFirstOperand().get();
			OperationElementStmt* secondOperand = multOper->getSecondOperand().get();
			checkOperationElement(expectedType, firstOperand, localScope, myClassScope, whoCalled, stmtName);
			checkOperationElement(expectedType, secondOperand, localScope, myClassScope, whoCalled, stmtName);
		}
	}
}

void SemanticAnalyzer::checkGetObjectAttribute(string expectedType, GetObjectAttributeStmt* objAttr, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled)
{
	string objectId = objAttr->getObjectId();
	string attributeId = objAttr->getAttributeId();
	string className = "";
	for (unsigned i = 0; i < localFunScope.size(); ++i) {
		if (objectId == localFunScope[i].id) {
			ClassInfo classScope = getMyClassInfo(localFunScope[i].type);
			if (classScope.classId == "")
				throw SemanticException("Class " + localFunScope[i].type + ", used in " + whoCalled + " function/class does not exist.");
			className = localFunScope[i].type;
			bool isFriend = false;
			for (unsigned j = 0; j < classScope.friendFunctions.size(); ++j) {
				if (whoCalled == classScope.friendFunctions[j])
					isFriend = true;
			}
			for (unsigned j = 0; j < classScope.friendClasses.size(); ++j) {
				if (myClassScope.classId == classScope.friendClasses[j])
					isFriend = true;
			}
			// mby function?
			for (unsigned j = 0; j < classScope.publicAttributes.size(); ++j) {
				if (attributeId == classScope.publicAttributes[j].id) {
					if (expectedType == "none")
						return;
					else if (classScope.publicAttributes[j].type != expectedType)
						throw SemanticException("Invalid type of attribute: " + classScope.publicAttributes[j].type + ", expected: " + expectedType + " in function/class" + whoCalled);
					return;
				}
			}
			for (unsigned j = 0; j < classScope.privateAttributes.size(); ++j) {
				if (attributeId == classScope.privateAttributes[j].id) {
					if (!isFriend)
						throw SemanticException("Cannot access private attribute: " + classScope.privateAttributes[j].id + " of class " + className + " in function/class " + whoCalled);
					if (expectedType == "none")
						return;
					else if (classScope.privateAttributes[j].type != expectedType)
						throw SemanticException("Invalid type of attribute: " + classScope.privateAttributes[j].type + ", expected: " + expectedType + " in function/class" + whoCalled);
					return;
				}
			}
			for (unsigned j = 0; j < classScope.inheritedAttributes.size(); ++j) {
				if (attributeId == classScope.inheritedAttributes[j].elemId) {
					if (expectedType == "none")
						return;
					else if (classScope.inheritedAttributes[j].type != expectedType)
						throw SemanticException("Invalid type of attribute: " + classScope.inheritedAttributes[j].type + ", expected: " + expectedType + " in function/class" + whoCalled);
					return;
				}
			}
			for (unsigned j = 0; j < classScope.privateInheritedAttributes.size(); ++j) {
				if (attributeId == classScope.privateInheritedAttributes[j].elemId) {
					if (!isFriend)
						throw SemanticException("Cannot access private attribute: " + classScope.privateInheritedAttributes[j].elemId + " in function/class" + whoCalled);
					if (expectedType == "none")
						return;
					else if (classScope.privateInheritedAttributes[j].type != expectedType)
						throw SemanticException("Invalid type of attribute " + classScope.privateInheritedAttributes[j].elemId + ": " + classScope.privateInheritedAttributes[j].type + ", expected: " + expectedType + " in function/class" + whoCalled);
					return;
				}
			}
		}
	}
	throw SemanticException("Undefined attribute: " + attributeId + " of class " + className + " used in function/class: " + whoCalled);
}

void SemanticAnalyzer::checkObjectMethodCall(string expectedType, ObjectMethodCallStmt* objMethodCall, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled)
{
	string objectId = objMethodCall->getObjectId();
	FunCallStmt* calledMethod = objMethodCall->getCalledMethod().get();
	CallArgsStmt* calledArgs = calledMethod->getCalledArgs().get();
	string methodId = calledMethod->getFunId();
	string className = "";

	for (unsigned i = 0; i < localFunScope.size(); ++i) {
		if (objectId == localFunScope[i].id) {
			ClassInfo classScope = getMyClassInfo(localFunScope[i].type);
			if (classScope.classId == "")
				throw SemanticException("Class " + localFunScope[i].type + ", used in " + whoCalled + " function/class does not exist.");
			className = localFunScope[i].type;
			bool isFriend = false;
			for (unsigned j = 0; j < classScope.friendFunctions.size(); ++j) {
				if (whoCalled == classScope.friendFunctions[j])
					isFriend = true;
			}
			for (unsigned j = 0; j < classScope.friendClasses.size(); ++j) {
				if (myClassScope.classId == classScope.friendClasses[j])
					isFriend = true;
			}
			// mby function?
			for (unsigned j = 0; j < classScope.publicMethods.size(); ++j) {
				if (methodId == classScope.publicMethods[j].id) {
					string expectedArgType;
					int expectedArgCount = classScope.publicMethods[j].arguments.size();
					if(calledArgs->getListOfArguments().size() != expectedArgCount)
						throw SemanticException("Invalid amount of arguments in function call: " + methodId + " called in " + whoCalled + " function/class. Provided: " +
							std::to_string(calledArgs->getListOfArguments().size()) + ", expected: " + std::to_string(expectedArgCount));
					for (unsigned k = 0; k < classScope.publicMethods[j].arguments.size(); ++k) {
						expectedArgType = classScope.publicMethods[j].arguments[k].type;
						checkOperationElement(expectedArgType, calledArgs->getListOfArguments()[k].get(), localFunScope, myClassScope, whoCalled, "method " + methodId + " call");
					}
					if (expectedType == "none")
						return;
					else if (classScope.publicMethods[j].type != expectedType)
						throw SemanticException("Invalid returned type: " + classScope.publicMethods[j].type + " of method: " + methodId + ", expected: " + expectedType + " in function/class" + whoCalled);
					return;
				}
			}
			for (unsigned j = 0; j < classScope.privateMethods.size(); ++j) {
				if (methodId == classScope.privateMethods[j].id) {
					if (!isFriend)
						throw SemanticException("Cannot access private method: " + classScope.privateMethods[j].id + " in function/class" + whoCalled);
					string expectedArgType;
					int expectedArgCount = classScope.privateMethods[j].arguments.size();
					if (calledArgs->getListOfArguments().size() != expectedArgCount)
						throw SemanticException("Invalid amount of arguments in function call: " + methodId + " called in " + whoCalled + " function/class. Provided: " +
							std::to_string(calledArgs->getListOfArguments().size()) + ", expected: " + std::to_string(expectedArgCount));
					for (unsigned k = 0; k < classScope.privateMethods[j].arguments.size(); ++k) {
						expectedArgType = classScope.privateMethods[j].arguments[k].type;
						checkOperationElement(expectedArgType, calledArgs->getListOfArguments()[k].get(), localFunScope, myClassScope, whoCalled, "method " + methodId + " call");
					}
					if (expectedType == "none")
						return;
					else if (classScope.privateMethods[j].type != expectedType)
						throw SemanticException("Invalid type of method " + classScope.privateMethods[j].id + ": " + classScope.privateMethods[j].type + ", expected: " + expectedType + " in function/class" + whoCalled);
					return;
				}
			}
			for (unsigned j = 0; j < classScope.inheritedMethods.size(); ++j) {
				if (methodId == classScope.inheritedMethods[j].id) {
					string expectedArgType;
					int expectedArgCount = classScope.inheritedMethods[j].arguments.size();
					if (calledArgs->getListOfArguments().size() != expectedArgCount)
						throw SemanticException("Invalid amount of arguments in function call: " + methodId + " called in " + whoCalled + " function/class. Provided: " +
							std::to_string(calledArgs->getListOfArguments().size()) + ", expected: " + std::to_string(expectedArgCount));
					for (unsigned k = 0; k < classScope.inheritedMethods[j].arguments.size(); ++k) {
						expectedArgType = classScope.inheritedMethods[j].arguments[k].type;
						checkOperationElement(expectedArgType, calledArgs->getListOfArguments()[k].get(), localFunScope, myClassScope, whoCalled, "method " + methodId + " call");
					}
					if (expectedType == "none")
						return;
					else if (classScope.inheritedMethods[j].type != expectedType)
						throw SemanticException("Invalid type of method " + classScope.inheritedMethods[j].id + ": " + classScope.inheritedMethods[j].type + ", expected: " + expectedType + " in function/class" + whoCalled);
					return;
				}
			}
			for (unsigned j = 0; j < classScope.privateInheritedMethods.size(); ++j) {
				if (methodId == classScope.privateInheritedMethods[j].id) {
					if (!isFriend)
						throw SemanticException("Cannot access private method: " + classScope.privateInheritedMethods[j].id + " in function/class" + whoCalled);
					string expectedArgType;
					int expectedArgCount = classScope.privateInheritedMethods[j].arguments.size();
					if (calledArgs->getListOfArguments().size() != expectedArgCount)
						throw SemanticException("Invalid amount of arguments in function call: " + methodId + " called in " + whoCalled + " function/class. Provided: " +
							std::to_string(calledArgs->getListOfArguments().size()) + ", expected: " + std::to_string(expectedArgCount));
					for (unsigned k = 0; k < classScope.privateInheritedMethods[j].arguments.size(); ++k) {
						expectedArgType = classScope.privateInheritedMethods[j].arguments[k].type;
						checkOperationElement(expectedArgType, calledArgs->getListOfArguments()[k].get(), localFunScope, myClassScope, whoCalled, "method " + methodId + " call");
					}
					if (expectedType == "none")
						return;
					else if (classScope.privateInheritedMethods[j].type != expectedType)
						throw SemanticException("Invalid type of method " + classScope.privateInheritedMethods[j].id + ": " + classScope.privateInheritedMethods[j].type + ", expected: " + expectedType + " in function/class" + whoCalled);
					return;
				}
			}
		}
		if(i == localFunScope.size() - 1)
			throw SemanticException("Undefined method: " + methodId + " of class  " + className + " used in function/class: " + whoCalled);
	}
	throw SemanticException("Undefined object: " + objectId + " used in function/class: " + whoCalled);
}

void SemanticAnalyzer::checkFunCall(string expectedType, FunCallStmt* funCall, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled)
{
	vector<string> typesOfArgs;
	CallArgsStmt* funCallArgs = funCall->getCalledArgs().get();
	string funId = funCall->getFunId();
	for (unsigned i = 0; i < localFunScope.size(); ++i) {
		if (funId == localFunScope[i].id) {
			if (expectedType == "none")
				return;
			else if (expectedType == localFunScope[i].type)
				return;
		}
	}
	if (findMethodInClassScope(funCallArgs, expectedType, funId, myClassScope, whoCalled))
		return;
	for (unsigned i = 0; i < globalElements.size(); ++i) {
		// when checking identifier, only function cares me from global range
		if (globalElements[i].type == "function") {
			if (funId == globalElements[i].myFunInfo.id) {
				string expectedArgType;
				unsigned expectedArgCount = globalElements[i].myFunInfo.arguments.size();
				if(funCallArgs->getListOfArguments().size() != expectedArgCount)
					throw SemanticException("Invalid amount of arguments in function call: " + funId + " called in " + whoCalled + " function/class. Provided: " +
						std::to_string(funCallArgs->getListOfArguments().size()) + ", expected: " + std::to_string(expectedArgCount));
				for (unsigned j = 0; j < expectedArgCount; ++j) {
					expectedArgType = globalElements[i].myFunInfo.arguments[j].type;
					checkOperationElement(expectedArgType, funCallArgs->getListOfArguments()[j].get(), localFunScope, myClassScope, whoCalled, "function " + funId);
				}
				if (expectedType == "none")
					return;
				else if (expectedType == globalElements[i].myFunInfo.type)
					return;
			}
		}
	}
	throw SemanticException("Undefined function/method: " + funId + " used in " + whoCalled + " function/class");
}

string SemanticAnalyzer::checkAssignStmt(AssignStmt* assignStmt, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled)
{
	string type;
	string assigneeId = assignStmt->getAssigneeId();
	type = findIdentifiersType(assigneeId, localFunScope, myClassScope, whoCalled);
	return type;
}

void SemanticAnalyzer::checkObjectInit(ObjectInitStmt* objInit, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled)
{
	string className = objInit->getClassName();
	string objectId = objInit->getObjectId();
	
	if (className == myClassScope.classId)
		throw SemanticException("Cannot instatiate an object of a " + className + " in it's own method or body.");

	// i am just checking if such a class is declared somewhere
	for (unsigned i = 0; i < localFunScope.size(); ++i) {
		if (className == localFunScope[i].type)
			return;
	}
	Element found = findAttributeInClassScope(className, className, myClassScope, whoCalled);
	if (found.type != "")
		return;
	for (unsigned i = 0; i < globalElements.size(); ++i) {
		if (globalElements[i].type == "class" && globalElements[i].myClassInfo.classId == className)
			return;
	}
	throw SemanticException("Undefined class: " + className + " in function/class " + whoCalled + " scope.");
}

Element SemanticAnalyzer::checkIdentifier(string expectedType, string calledAttribute, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled, string stmtName)
{
	Element found;
	for (unsigned i = 0; i < localFunScope.size(); ++i) {
		if (calledAttribute == localFunScope[i].id) {
			found.id = localFunScope[i].id;
			if (expectedType == "none") {
				found.type = "none";
				return found;
			}
			else if (expectedType == localFunScope[i].type) {
				found.type = expectedType;
				return found;
			}
			throw SemanticException("Invalid type of identifier: " + calledAttribute + " used in " + stmtName + " in class " + myClassScope.classId + " in " + whoCalled + " method.");
		}
	}
	found = findAttributeInClassScope(expectedType, calledAttribute, myClassScope, whoCalled);
	if (found.id != "")
		return found;
	for (unsigned i = 0; i < globalElements.size(); ++i) {
		// when checking identifier, only function cares me from global range
		if (globalElements[i].type == "function") {
			if (calledAttribute == globalElements[i].myFunInfo.id) {
				found.id = globalElements[i].myFunInfo.id;
				if (expectedType == "none") {
					found.type = "none";
					return found;
				}
				else if (expectedType == globalElements[i].myFunInfo.type) {
					found.type = expectedType;
					return found;
				}
			}
		}
	}
	throw SemanticException("Undefined identifier: " + calledAttribute + " used in " + whoCalled + " function/class");
}

Element SemanticAnalyzer::findAttributeInClassScope(string expectedType, string calledAttribute, ClassInfo myClassScope, string whoCalled)
{
	Element found;
	for (unsigned i = 0; i < myClassScope.privateAttributes.size(); ++i) {
		if (calledAttribute == myClassScope.privateAttributes[i].id) {
			found.id = calledAttribute;
			if (expectedType == "none") {
				found.id = "none";
				return found;
			}
			else if (expectedType == myClassScope.privateAttributes[i].type) {
				found.id = expectedType;
				return found;
			}
		}
	}
	for (unsigned i = 0; i < myClassScope.publicAttributes.size(); ++i) {
		if (calledAttribute == myClassScope.publicAttributes[i].id) {
			found.id = calledAttribute;
			if (expectedType == "none") {
				found.id = "none";
				return found;
			}
			else if (expectedType == myClassScope.publicAttributes[i].type) {
				found.id = expectedType;
				return found;
			}
		}
	}
	for (unsigned i = 0; i < myClassScope.inheritedAttributes.size(); ++i) {
		if (calledAttribute == myClassScope.inheritedAttributes[i].elemId) {
			found.id = calledAttribute;
			if (expectedType == "none") {
				found.id = "none";
				return found;
			}
			else if (expectedType == myClassScope.inheritedAttributes[i].type) {
				found.id = expectedType;
				return found;
			}
		}
	}
	for (unsigned i = 0; i < myClassScope.privateInheritedAttributes.size(); ++i) {
		if (calledAttribute == myClassScope.privateInheritedAttributes[i].elemId) {
			found.id = calledAttribute;
			if (expectedType == "none") {
				found.id = "none";
				return found;
			}
			else if (expectedType == myClassScope.privateInheritedAttributes[i].type) {
				found.id = expectedType;
				return found;
			}
		}
	}
	return found;
}

string SemanticAnalyzer::findIdentifiersType(string identifier, const vector<Element>& localFunScope, ClassInfo myClassScope, string whoCalled)
{
	string found;
	for (unsigned i = 0; i < localFunScope.size(); ++i) {
		if (identifier == localFunScope[i].id)
			return localFunScope[i].type;
	}
	found = findTypeInClassScope(identifier, myClassScope);
	if (found != "")
		return found;

	for (unsigned i = 0; i < globalElements.size(); ++i) {
		// when checking identifier, only function cares me from global range
		if (globalElements[i].type == "function") {
			if (identifier == globalElements[i].myFunInfo.id) {
				return globalElements[i].myFunInfo.type;
			}
		}
	}
	throw SemanticException("Undefined identifier: " + identifier + " used in " + whoCalled + " function/class");
}

string SemanticAnalyzer::findTypeInClassScope(string identifier, ClassInfo myClassScope)
{
	for (unsigned i = 0; i < myClassScope.privateAttributes.size(); ++i) {
		if (identifier == myClassScope.privateAttributes[i].id) {
			return myClassScope.privateAttributes[i].type;
		}
	}
	for (unsigned i = 0; i < myClassScope.publicAttributes.size(); ++i) {
		if (identifier == myClassScope.publicAttributes[i].id) {
			return myClassScope.publicAttributes[i].type;
		}
	}
	for (unsigned i = 0; i < myClassScope.inheritedAttributes.size(); ++i) {
		if (identifier == myClassScope.inheritedAttributes[i].elemId) {
			return myClassScope.inheritedAttributes[i].type; 
		}
	}
	for (unsigned i = 0; i < myClassScope.privateInheritedAttributes.size(); ++i) {
		if (identifier == myClassScope.privateInheritedAttributes[i].elemId) {
			return myClassScope.privateInheritedAttributes[i].type;
		}
	}
	return "";
}

bool SemanticAnalyzer::findMethodInClassScope(CallArgsStmt* funCallArgs, string expectedType, string calledMethod, ClassInfo myClassScope, string whoCalled)
{
	vector<Element> emptyLocalScope;
	for (unsigned i = 0; i < myClassScope.privateMethods.size(); ++i) {
		if (calledMethod == myClassScope.privateMethods[i].id) {
			string expectedArgType;
			int expectedArgCount = myClassScope.privateMethods[i].arguments.size();
			if (funCallArgs->getListOfArguments().size() != expectedArgCount)
				throw SemanticException("Invalid amount of arguments in function call: " + calledMethod + " called in " + whoCalled + " function/class. Provided: " +
				std::to_string(funCallArgs->getListOfArguments().size()) + ", expected: " + std::to_string(expectedArgCount));
			for (unsigned j = 0; j < myClassScope.privateMethods[i].arguments.size(); ++j) {
				expectedArgType = myClassScope.privateMethods[i].arguments[j].type;
				checkOperationElement(expectedArgType, funCallArgs->getListOfArguments()[j].get(), emptyLocalScope, myClassScope, whoCalled, "method " + calledMethod + " call");
			}
			if (expectedType == "none")
				return true;
			else if (expectedType == myClassScope.privateMethods[i].type)
				return true;
		}
	}
	for (unsigned i = 0; i < myClassScope.publicMethods.size(); ++i) {
		if (calledMethod == myClassScope.publicMethods[i].id) {
			string expectedArgType;
			int expectedArgCount = myClassScope.publicMethods[i].arguments.size();
			if (funCallArgs->getListOfArguments().size() != expectedArgCount)
				throw SemanticException("Invalid amount of arguments in function call: " + calledMethod + " called in " + whoCalled + " function/class. Provided: " +
				std::to_string(funCallArgs->getListOfArguments().size()) + ", expected: " + std::to_string(expectedArgCount));
			for (unsigned j = 0; j < myClassScope.publicMethods[i].arguments.size(); ++j) {
				expectedArgType = myClassScope.publicMethods[i].arguments[j].type;
				checkOperationElement(expectedArgType, funCallArgs->getListOfArguments()[j].get(), emptyLocalScope, myClassScope, whoCalled, "method " + calledMethod + " call");
			}
			if (expectedType == "none")
				return true;
			else if (expectedType == myClassScope.publicMethods[i].type)
				return true;
		}
	}
	for (unsigned i = 0; i < myClassScope.inheritedMethods.size(); ++i) {
		if (calledMethod == myClassScope.inheritedMethods[i].id) {
			string expectedArgType;
			int expectedArgCount = myClassScope.inheritedMethods[i].arguments.size();
			if (funCallArgs->getListOfArguments().size() != expectedArgCount)
				throw SemanticException("Invalid amount of arguments in function call: " + calledMethod + " called in " + whoCalled + " function/class. Provided: " +
					std::to_string(funCallArgs->getListOfArguments().size()) + ", expected: " + std::to_string(expectedArgCount));
			for (unsigned j = 0; j < myClassScope.inheritedMethods[i].arguments.size(); ++j) {
				expectedArgType = myClassScope.inheritedMethods[i].arguments[j].type;
				checkOperationElement(expectedArgType, funCallArgs->getListOfArguments()[j].get(), emptyLocalScope, myClassScope, whoCalled, "method " + calledMethod + " call");
			}
			if (expectedType == "none")
				return true;
			else if (expectedType == myClassScope.inheritedMethods[i].type)
				return true;
		}
	}
	for (unsigned i = 0; i < myClassScope.privateInheritedMethods.size(); ++i) {
		if (calledMethod == myClassScope.privateInheritedMethods[i].id) {
			string expectedArgType;
			int expectedArgCount = myClassScope.privateInheritedMethods[i].arguments.size();
			if (funCallArgs->getListOfArguments().size() != expectedArgCount)
				throw SemanticException("Invalid amount of arguments in function call: " + calledMethod + " called in " + whoCalled + " function/class. Provided: " +
					std::to_string(funCallArgs->getListOfArguments().size()) + ", expected: " + std::to_string(expectedArgCount));
			for (unsigned j = 0; j < myClassScope.privateInheritedMethods[i].arguments.size(); ++j) {
				expectedArgType = myClassScope.privateInheritedMethods[i].arguments[j].type;
				checkOperationElement(expectedArgType, funCallArgs->getListOfArguments()[j].get(), emptyLocalScope, myClassScope, whoCalled, "method " + calledMethod + " call");
			}
			if (expectedType == "none")
				return true;
			else if (expectedType == myClassScope.privateInheritedMethods[i].type)
				return true;
		}
	}
	return false;
}

void SemanticAnalyzer::addArgsToLocalFunScope(FunDefinitionStmt* funDef, vector<Element>& localFunScope)
{
	string stmtName;
	ArgumentsStmt* funArgs = funDef->getArgs().get();
	for (unsigned i = 0; i < funArgs->getSignatures().size(); ++i) {
		SignatureStmt* sig = funArgs->getSignatures()[i].get();
		stmtName = sig->getStmtClassName();
		Element newElem;
		if (stmtName == "class ClassSignatureStmt *") {
			ClassSignatureStmt* classSig = dynamic_cast<ClassSignatureStmt*>(sig);
			newElem.type = classSig->getMyClassName();
			newElem.id = classSig->getMyId();
		}
		else if (stmtName == "class SignatureStmt *") {
			newElem.type = setType(sig->getMyType());
			newElem.id = sig->getMyId();
		}
		localFunScope.push_back(newElem);
	}
}

ClassInfo SemanticAnalyzer::getMyClassInfo(string myId)
{
	for (unsigned i = 0; i < classesInfo.size(); ++i) {
		if (classesInfo[i].classId == myId)
			return classesInfo[i];
	}
	std::cerr << "No such a class defined" << std::endl;
	ClassInfo info;
	return info;	// returns empty info struct
}
