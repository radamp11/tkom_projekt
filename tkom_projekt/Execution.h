#pragma once
#include <iomanip>
#include "Statement.h"

using std::setw;

const int CLASS_ID_COLUMN = 10;
const int INHERITS_COLUMN = 15;
const int FRIEND_CLASSES_COLUMN = 15;
const int FRIEND_FUNCTIONS_COLUMN = 15;


struct Element {
	string type = "";
	string id = "";
};


struct FunInfo {
	string type = "";
	string id = "";
	vector<Element> arguments;
	string ownerId = "";
};


struct InheritedAttr {
	string type = "";
	string elemId = "";
	string ownerId = "";
};


struct ClassInfo {
	string classId = "";
	vector<InheritedClass> inherits;
	vector<string> friendClasses;
	vector<string> friendFunctions;
	// my private attributes
	vector<Element> privateAttributes;
	vector<FunInfo> privateMethods;
	// my public attributes
	vector<Element> publicAttributes;
	vector<FunInfo> publicMethods;
	//inherited attributes
	vector<InheritedAttr> inheritedAttributes;
	vector<FunInfo> inheritedMethods;
	vector<InheritedAttr> privateInheritedAttributes;
	vector<FunInfo> privateInheritedMethods;

	void printMe() {
		cout << setw(CLASS_ID_COLUMN) << classId << "|";
		string tmp = "";
		for (unsigned i = 0; i < inherits.size(); ++i) {
			tmp += inherits[i].className;
			if (i != inherits.size() - 1)
				tmp += ", ";
		}
		cout << setw(INHERITS_COLUMN) << tmp << "|";
		tmp = "";
		for (unsigned i = 0; i < friendClasses.size(); ++i) {
			tmp += friendClasses[i];
			if (i != friendClasses.size() - 1)
				tmp += ", ";
		}
		cout << setw(FRIEND_CLASSES_COLUMN) << tmp << "|";
		tmp = "";
		for (unsigned i = 0; i < friendFunctions.size(); ++i) {
			tmp += friendFunctions[i];
			if (i != friendFunctions.size() - 1)
				tmp += ", ";
		}
		cout << setw(FRIEND_FUNCTIONS_COLUMN) << tmp << endl;
	}
};


struct CallingElement {
	string callingClassId;
	string callingMethodId;

	string calledClassId;
	string calledMethodId = "";
	string calledAttributeId = "";

	string initOrMethod;

	void printMe() {
		cout << "Class: " << callingClassId << " in its " << initOrMethod << ": " << callingMethodId
			<< " calls ";
		if (calledMethodId == "" && calledAttributeId != "")
			cout << "an attribute: " << calledAttributeId;
		else if (calledMethodId != "" && calledAttributeId == "")
			cout << "a method: " << calledMethodId;
		cout << " of class " << calledClassId << endl;
	}
};


class Execution {
public:
	void run(vector<Statement_ptr>& programStatements) { buildTrees(programStatements); }
	void printResults() { printTrees(); }
	vector<ClassInfo>& getClassInfoTree() { return inhFriendTree; }
private:
	int treeCounter = 0;

	string setType(MyType type);

	void buildTrees(vector<Statement_ptr>& programStatements);
	void buildTree(vector<Statement_ptr>& programStatements, int option);
	//void buildCallingTree(vector<Statement_ptr>& programStatements);
	void printTrees();

	void checkFunBlock(BlockStmt* block, int option);
	void checkClassBlock(ClassDefinitionStmt* classDef, int option);
	void addNewIFelems(ClassDefinitionStmt* classDef);

	void addInheritedAttrs();
	void addCallings(ClassDefinitionStmt* classDef);

	void checkOperationElement(OperationElementStmt* assignedElem, vector<string>& identifiersToCheck);
	void addElems(string className, string statementId, vector<string>& identifiersToCheck, string whoCalled);
	void checkDefinedFun(FunDefinitionStmt* definedFun, string className);
	void checkCalledFun(FunCallStmt* calledFun, string className, vector<string>& identifiersToCheck);
	void checkReturnStmt(ReturnStmt* returnedStmt, vector<string>& identifiersToCheck);

	vector<ClassInfo> inhFriendTree;
	vector<CallingElement> callingTree;
};
