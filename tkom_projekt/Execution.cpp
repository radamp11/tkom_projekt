#include "Execution.h"

string Execution::setType(MyType type) {
    string stringType;
    if (type == _int)
        stringType = "int";
    else if (type == _char)
        stringType = "char";
    else if (type == _void)
        stringType = "void";
    return stringType;
}

void Execution::buildTrees(vector<Statement_ptr>& programStatements) {
    buildTree(programStatements, 1);
    buildGlobalClasses(programStatements);
    addInheritedElemsId(programStatements, nullptr);
    addInheritedAttrs();
    buildTree(programStatements, 2);
    treeCounter = 0;
}

void Execution::buildTree(vector<Statement_ptr>& programStatements, int option) {
    string stmtName;

    for (unsigned i = 0; i < programStatements.size(); ++i) {
        stmtName = programStatements[i]->getStmtClassName();
        if (stmtName == "class ClassDefinitionStmt *") {
            ClassDefinitionStmt* classDef = dynamic_cast<ClassDefinitionStmt*>(programStatements[i].get());
            checkClassBlock(classDef, option);
        }
        else if (stmtName == "class FunDefinitionStmt *") {
            FunDefinitionStmt* funDef = dynamic_cast<FunDefinitionStmt*>(programStatements[i].get());
            BlockStmt* funBlock = funDef->getBlock().get();
            checkFunBlock(funBlock, option);
        }
    }
}

ClassInfo& Execution::getMyClassInfo(string myId, int myUniqueId) {
    for (unsigned i = 0; i < inhFriendTree.size(); ++i) {
        if (inhFriendTree[i].classId == myId && inhFriendTree[i].uniqueId == myUniqueId)
            return inhFriendTree[i];
    }
    std::cerr << "No such a class defined" << std::endl;
    ClassInfo info;
    return info; // returns empty info struct
}

void Execution::buildGlobalClasses(vector<Statement_ptr>& programStatements) {
    string stmtName;
    for (unsigned i = 0; i < programStatements.size(); ++i) {
        stmtName = programStatements[i]->getStmtClassName();
        if (stmtName == "class ClassDefinitionStmt *") {
            ClassDefinitionStmt* classDef = dynamic_cast<ClassDefinitionStmt*>(programStatements[i].get());
            ClassInfo myClassInfo = getMyClassInfo(classDef->getName(), classDef->getUniqueId());
            globalClasses.push_back(myClassInfo);
        }
    }
}

void Execution::addInheritedElemsId(vector<Statement_ptr>& programStatements, ClassDefinitionStmt* motherClassDef) {
    InheritedClassInfo newInfo;
    string stmtName;
    for (unsigned i = 0; i < programStatements.size(); ++i) {
        stmtName = programStatements[i]->getStmtClassName();
        if (stmtName == "class ClassDefinitionStmt *") {
            ClassDefinitionStmt* classDef = dynamic_cast<ClassDefinitionStmt*>(programStatements[i].get());
            analyzeInheritance(classDef, motherClassDef);
        }
        else if (stmtName == "class FunDefinitionStmt *") {
            FunDefinitionStmt* funDef = dynamic_cast<FunDefinitionStmt*>(programStatements[i].get());
            BlockStmt* funBlock = funDef->getBlock().get();
            addInheritedElemsId(funBlock->getStatements(), motherClassDef);
        }
    }
}

void Execution::analyzeInheritance(ClassDefinitionStmt* classDef, ClassDefinitionStmt* motherClassDef) {
    BlockStmt* classBlock = classDef->getDeclarationBlock().get();
    addInheritedElemsId(classBlock->getStatements(), classDef); // check classes of this class
    if (classDef->getInheritedClasses().empty())
        return;

    string stmtName;
    vector<InheritedClassInfo>& inhClassInfo = getMyClassInfo(classDef->getName(), classDef->getUniqueId()).inherits;
    unsigned foundClasses = 0;

    if (motherClassDef) {
        BlockStmt* motherClassBlock = motherClassDef->getDeclarationBlock().get();
        vector<ClassInfo> localClassesScope;

        for (unsigned i = 0; i < motherClassBlock->getStatements().size(); ++i) {
            stmtName = motherClassBlock->getStatements()[i]->getStmtClassName();
            if (stmtName == "class ClassDefinitionStmt *") {
                ClassDefinitionStmt* classDefinition =
                    dynamic_cast<ClassDefinitionStmt*>(motherClassBlock->getStatements()[i].get());
                localClassesScope.push_back(getMyClassInfo(classDefinition->getName(), classDefinition->getUniqueId()));
            }
        } // this is my local scope of potential classes, that i am inheriting after
        // now check if some of them matches

        for (unsigned i = 0; i < inhClassInfo.size(); ++i) {
            for (unsigned j = 0; j < localClassesScope.size(); ++j) {
                if (inhClassInfo[i].className == localClassesScope[j].classId) {
                    inhClassInfo[i].uniqueId = localClassesScope[j].uniqueId;
                    foundClasses++;
                }
            }
        }
        if (foundClasses == inhClassInfo.size())
            return;
    }

    // if not satisfied, check global scope

    for (unsigned i = 0; i < inhClassInfo.size(); ++i) {
        for (unsigned j = 0; j < globalClasses.size(); ++j) {
            if (inhClassInfo[i].className == globalClasses[j].classId && inhClassInfo[i].uniqueId == -1) {
                inhClassInfo[i].uniqueId = globalClasses[j].uniqueId;
                foundClasses++;
            }
        }
    }
    if (foundClasses == inhClassInfo.size())
        return;

    string unfoundClass;
    for (unsigned i = 0; i < inhClassInfo.size(); ++i)
        if (inhClassInfo[i].uniqueId == -1)
            unfoundClass = inhClassInfo[i].className;
    throw SemanticException("Inherited class:" + unfoundClass +
        " is undefined or out of scope (definition of class: " + classDef->getName() + ").");
}

// in this function i only check occurrences of new ClassDefinitionStmt 's

void Execution::checkFunBlock(BlockStmt* block, int opt) {
    string stmtName;
    for (unsigned i = 0; i < block->getStatements().size(); ++i) {
        stmtName = block->getStatements()[i]->getStmtClassName();
        if (stmtName == "class ClassDefinitionStmt *") {
            ClassDefinitionStmt* classDef = dynamic_cast<ClassDefinitionStmt*>(block->getStatements()[i].get());
            checkClassBlock(classDef, opt);
        }
    }
}

// in this function i look for new ClassDefinitionStmt's, FunDefinitionStmt's
// and friendships, so fun and class declarations, and then checking isFriend flag

void Execution::checkClassBlock(ClassDefinitionStmt* classDef, int opt) {
    BlockStmt* block = classDef->getDeclarationBlock().get();
    buildTree(block->getStatements(), opt); // builds new elems inside this class
                                            // gets to the leafs of statements tree
    if (opt == 1)
        addNewIFelems(classDef);
    else if (opt == 2)
        addCallings(classDef);
}

void Execution::addNewIFelems(ClassDefinitionStmt* classDef) {
    string stmtName;
    ClassInfo newIFelem;
    newIFelem.uniqueId = classDef->getUniqueId();
    newIFelem.classId = classDef->getName();

    for (unsigned i = 0; i < classDef->getInheritedClasses().size(); ++i) {
        InheritedClassInfo newInfo;
        newInfo.access = classDef->getInheritedClasses()[i].access;
        newInfo.className = classDef->getInheritedClasses()[i].className;
        newIFelem.inherits.push_back(newInfo);
    }

    BlockStmt* classBlock = classDef->getDeclarationBlock().get();

    for (unsigned i = 0; i < classBlock->getStatements().size(); ++i) {
        stmtName = classBlock->getStatements()[i]->getStmtClassName();
        if (stmtName == "class ClassDeclarationStmt *") {
            ClassDeclarationStmt* classDec = dynamic_cast<ClassDeclarationStmt*>(classBlock->getStatements()[i].get());
            if (classDec->getIsFriend())
                newIFelem.friendClasses.push_back(classDec->getClassId());
        }
        else if (stmtName == "class FunDeclarationStmt *") {
            FunDeclarationStmt* funDec = dynamic_cast<FunDeclarationStmt*>(classBlock->getStatements()[i].get());
            if (funDec->getIsFriend())
                newIFelem.friendFunctions.push_back(funDec->getFunId());
        }
        else if (stmtName == "class InitStmt *") {
            InitStmt* initStmt = dynamic_cast<InitStmt*>(classBlock->getStatements()[i].get());
            Element newElement;
            newElement.id = initStmt->getId();
            newElement.type = setType(initStmt->getReturnedType());
            if (initStmt->getAccessModifier() == 1)
                newIFelem.publicAttributes.push_back(newElement);
            else if (initStmt->getAccessModifier() == -1)
                newIFelem.privateAttributes.push_back(newElement);
        }
        else if (stmtName == "class FunDefinitionStmt *") {
            FunDefinitionStmt* funDef = dynamic_cast<FunDefinitionStmt*>(classBlock->getStatements()[i].get());
            FunInfo newClassMethod;
            newClassMethod.id = funDef->getFunId();
            newClassMethod.type = setType(funDef->getReturnedType());
            ArgumentsStmt* funArgs = funDef->getArgs().get();
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
                newClassMethod.arguments.push_back(elem);
            }
            if (funDef->getAccessModifier() == 1)
                newIFelem.publicMethods.push_back(newClassMethod);
            else if (funDef->getAccessModifier() == -1)
                newIFelem.privateMethods.push_back(newClassMethod);
        }
        else if (stmtName == "class ClassDefinitionStmt *") {
            ClassDefinitionStmt* classDef = dynamic_cast<ClassDefinitionStmt*>(classBlock->getStatements()[i].get());
            Element newElement;
            newElement.id = classDef->getName();
            newElement.type = classDef->getName();
            newElement.owner.ownerName = classDef->getName();
            newElement.owner.ownerId = classDef->getUniqueId();
            if (classDef->getAccessModifier() == 1)
                newIFelem.publicAttributes.push_back(newElement);
            else if (classDef->getAccessModifier() == -1)
                newIFelem.privateAttributes.push_back(newElement);
        }
        else if (stmtName == "class ObjectInitStmt *") {
            ObjectInitStmt* objectInit = dynamic_cast<ObjectInitStmt*>(classBlock->getStatements()[i].get());
            Element newElement;
            newElement.id = objectInit->getObjectId();
            newElement.type = objectInit->getClassName();
            if (objectInit->getAccessModifier() == 1)
                newIFelem.publicAttributes.push_back(newElement);
            else if (objectInit->getAccessModifier() == -1)
                newIFelem.privateAttributes.push_back(newElement);
        }
    }
    inhFriendTree.push_back(newIFelem);
}

// goes through already built tree and puts info about inherited methods and attributes

void Execution::addInheritedAttrs() {
    // this class we update
    for (unsigned i = 0; i < inhFriendTree.size(); ++i) {
        if (inhFriendTree[i].inherits.empty())
            continue;
        // with methods and attributes of inherited classes
        for (unsigned j = 0; j < inhFriendTree[i].inherits.size(); ++j) {
            bool isPrivate = false;
            if (inhFriendTree[i].inherits[j].access == "private")
                isPrivate = true;
            string currId = inhFriendTree[i].inherits[j].className;
            int currUniqieId = inhFriendTree[i].inherits[j].uniqueId;
            // find inherited class info
            for (unsigned k = 0; k < inhFriendTree.size(); ++k) {
                if (inhFriendTree[k].classId == currId && inhFriendTree[k].uniqueId == currUniqieId) {
                    InheritedAttr elem;
                    // and add it

                    for (unsigned l = 0; l < inhFriendTree[k].publicAttributes.size(); ++l) {
                        elem.type = inhFriendTree[k].publicAttributes[l].type;
                        elem.elemId = inhFriendTree[k].publicAttributes[l].id;
                        elem.owner.ownerName = inhFriendTree[k].classId;
                        elem.owner.ownerId = inhFriendTree[k].uniqueId;
                        if (isPrivate)
                            inhFriendTree[i].privateInheritedAttributes.push_back(elem);
                        else
                            inhFriendTree[i].inheritedAttributes.push_back(elem);
                    }
                    FunInfo method;
                    for (unsigned l = 0; l < inhFriendTree[k].publicMethods.size(); ++l) {
                        method.type = inhFriendTree[k].publicMethods[l].type;
                        method.id = inhFriendTree[k].publicMethods[l].id;
                        method.owner.ownerName = inhFriendTree[k].classId;
                        method.owner.ownerId = inhFriendTree[k].uniqueId;
                        method.arguments = inhFriendTree[k].publicMethods[l].arguments;
                        if (isPrivate)
                            inhFriendTree[i].privateInheritedMethods.push_back(method);
                        else
                            inhFriendTree[i].inheritedMethods.push_back(method);
                    }
                    for (unsigned l = 0; l < inhFriendTree[k].inheritedAttributes.size(); ++l) {
                        if (isPrivate)
                            inhFriendTree[i].privateInheritedAttributes.push_back(inhFriendTree[k].inheritedAttributes[l]);
                        else
                            inhFriendTree[i].inheritedAttributes.push_back(inhFriendTree[k].inheritedAttributes[l]);
                    }
                    for (unsigned l = 0; l < inhFriendTree[k].inheritedMethods.size(); ++l) {
                        if (isPrivate)
                            inhFriendTree[i].privateInheritedMethods.push_back(inhFriendTree[k].inheritedMethods[l]);
                        else
                            inhFriendTree[i].inheritedMethods.push_back(inhFriendTree[k].inheritedMethods[l]);
                    }
                    break;
                }
            }
        }
    }
}

void Execution::addCallings(ClassDefinitionStmt* classDef) {
    string stmtName;
    string className = classDef->getName();
    if (className != inhFriendTree[treeCounter].classId)
        throw "Error in generating callingTree.\n";

    BlockStmt* classBlock = classDef->getDeclarationBlock().get();

    for (unsigned i = 0; i < classBlock->getStatements().size(); ++i) {
        stmtName = classBlock->getStatements()[i]->getStmtClassName();
        if (stmtName == "class InitStmt *") {
            InitStmt* initStmt = dynamic_cast<InitStmt*>(classBlock->getStatements()[i].get());
            OperationElementStmt* assignedElem = initStmt->getAssignedElement().get();
            vector<string> identifiersToCheck;
            // add all possible identifiers in this stmt
            if (assignedElem)
                checkOperationElement(assignedElem, identifiersToCheck);
            // and then check them if they match inherited methods, add if yes
            addElems(className, initStmt->getId(), identifiersToCheck, "init statement");
        }
        else if (stmtName == "class AssignStmt *") {
            AssignStmt* assignStmt = dynamic_cast<AssignStmt*>(classBlock->getStatements()[i].get());
            OperationElementStmt* assignedElem = assignStmt->getAssignedStatement().get();
            vector<string> identifiersToCheck;
            if (assignedElem)
                checkOperationElement(assignedElem, identifiersToCheck);
            addElems(className, assignStmt->getAssigneeId(), identifiersToCheck, "assign statement");
        }
        // else if (stmtName == "class FunCallStmt *") {
        //	FunCallStmt* calledFun = dynamic_cast<FunCallStmt*>(classBlock->getStatements()[i].get());
        // check id itself and called args
        //	checkCalledFun(calledFun, );
        //}
        else if (stmtName == "class FunDefinitionStmt *") {
            FunDefinitionStmt* funDef = dynamic_cast<FunDefinitionStmt*>(classBlock->getStatements()[i].get());
            // check fun block
            checkDefinedFun(funDef, className);
        }
    }

    treeCounter++;
}

void Execution::checkOperationElement(OperationElementStmt* assignedElem, vector<string>& identifiersToCheck) {

    string* calledAttribute = std::get_if<string>(assignedElem->getOperationElement());
    if (calledAttribute) { // it is an attribute called
        identifiersToCheck.push_back(*calledAttribute);
    }
    else { // it is an expression
        auto exprPtr = std::get_if<ExpressionStmt_ptr>(assignedElem->getOperationElement());
        if (exprPtr == nullptr)
            return;
        ExpressionStmt* expression = exprPtr->get();
        // checking kind of expression
        FunCallStmt* funCall = dynamic_cast<FunCallStmt*>(expression);
        AddOperationStmt* addOper = dynamic_cast<AddOperationStmt*>(expression);
        MultOperationStmt* multOper = dynamic_cast<MultOperationStmt*>(expression);
        // if it is not one of expressions above, dynamic_cast returns nullptr
        if (funCall) {
            identifiersToCheck.push_back(funCall->getFunId());
        }
        else if (addOper) {
            OperationElementStmt* firstOperand = addOper->getFirstOperand().get();
            OperationElementStmt* secondOperand = addOper->getSecondOperand().get();
            checkOperationElement(firstOperand, identifiersToCheck);
            checkOperationElement(secondOperand, identifiersToCheck);
        }
        else if (multOper) {
            OperationElementStmt* firstOperand = multOper->getFirstOperand().get();
            OperationElementStmt* secondOperand = multOper->getSecondOperand().get();
            checkOperationElement(firstOperand, identifiersToCheck);
            checkOperationElement(secondOperand, identifiersToCheck);
        } // rest opitons are not needed
    }
}

void Execution::addElems(string className, string statementId, vector<string>& identifiersToCheck, string whoCalled) {
    for (unsigned i = 0; i < identifiersToCheck.size(); ++i) {
        for (unsigned j = 0; j < inhFriendTree[treeCounter].inheritedAttributes.size(); ++j) {
            CallingElement newCall;
            if (identifiersToCheck[i] == inhFriendTree[treeCounter].inheritedAttributes[j].elemId) {
                newCall.callingClassId = className;
                newCall.callingMethodId = statementId;
                newCall.calledAttributeId = inhFriendTree[treeCounter].inheritedAttributes[j].elemId;
                newCall.calledClassId = inhFriendTree[treeCounter].inheritedAttributes[j].owner.ownerName;
                newCall.initOrMethod = whoCalled;
                callingTree.push_back(newCall);
            }
        }
        for (unsigned j = 0; j < inhFriendTree[treeCounter].inheritedMethods.size(); ++j) {
            CallingElement newCall;
            if (identifiersToCheck[i] == inhFriendTree[treeCounter].inheritedMethods[j].id) {
                newCall.callingClassId = className;
                newCall.callingMethodId = statementId;
                newCall.calledMethodId = inhFriendTree[treeCounter].inheritedMethods[j].id;
                newCall.calledClassId = inhFriendTree[treeCounter].inheritedMethods[j].owner.ownerName;
                newCall.initOrMethod = whoCalled;
                callingTree.push_back(newCall);
            }
        }
        for (unsigned j = 0; j < inhFriendTree[treeCounter].privateInheritedAttributes.size(); ++j) {
            CallingElement newCall;
            if (identifiersToCheck[i] == inhFriendTree[treeCounter].privateInheritedAttributes[j].elemId) {
                newCall.callingClassId = className;
                newCall.callingMethodId = statementId;
                newCall.calledAttributeId = inhFriendTree[treeCounter].privateInheritedAttributes[j].elemId;
                newCall.calledClassId = inhFriendTree[treeCounter].privateInheritedAttributes[j].owner.ownerName;
                newCall.initOrMethod = whoCalled;
                callingTree.push_back(newCall);
            }
        }
        for (unsigned j = 0; j < inhFriendTree[treeCounter].privateInheritedMethods.size(); ++j) {
            CallingElement newCall;
            if (identifiersToCheck[i] == inhFriendTree[treeCounter].privateInheritedMethods[j].id) {
                newCall.callingClassId = className;
                newCall.callingMethodId = statementId;
                newCall.calledMethodId = inhFriendTree[treeCounter].privateInheritedMethods[j].id;
                newCall.calledClassId = inhFriendTree[treeCounter].privateInheritedMethods[j].owner.ownerName;
                newCall.initOrMethod = whoCalled;
                callingTree.push_back(newCall);
            }
        }
    }
}

void Execution::checkDefinedFun(FunDefinitionStmt* definedFun, string className) {
    string stmtName;
    vector<string> identifiersToCheck;
    BlockStmt* funBlock = definedFun->getBlock().get();

    // collect all calls in this method
    for (unsigned i = 0; i < funBlock->getStatements().size(); ++i) {
        stmtName = funBlock->getStatements()[i]->getStmtClassName();
        if (stmtName == "class InitStmt *") {
            InitStmt* initStmt = dynamic_cast<InitStmt*>(funBlock->getStatements()[i].get());
            OperationElementStmt* assignedElem = initStmt->getAssignedElement().get();
            vector<string> identifiersToCheck;
            // add all possible identifiers in this stmt
            checkOperationElement(assignedElem, identifiersToCheck);
            // and then check them if they match inherited methods, add if yes
            // addElems(className, initStmt->getId(), identifiersToCheck);
        }
        else if (stmtName == "class AssignStmt *") {
            AssignStmt* assignStmt = dynamic_cast<AssignStmt*>(funBlock->getStatements()[i].get());
            OperationElementStmt* assignedElem = assignStmt->getAssignedStatement().get();
            checkOperationElement(assignedElem, identifiersToCheck);
            // addElems(className, assignStmt->getAssigneeId(), identifiersToCheck);
        }
        else if (stmtName == "class FunCallStmt *") {
            FunCallStmt* calledFun = dynamic_cast<FunCallStmt*>(funBlock->getStatements()[i].get());
            // check id itself and called args
            checkCalledFun(calledFun, className, identifiersToCheck);
        }
        else if (stmtName == "class ReturnStmt *") {
            ReturnStmt* returnedStmt = dynamic_cast<ReturnStmt*>(funBlock->getStatements()[i].get());
            checkReturnStmt(returnedStmt, identifiersToCheck);
        }
    }
    addElems(className, definedFun->getFunId(), identifiersToCheck, "method");
}

void Execution::checkCalledFun(FunCallStmt* calledFun, string className, vector<string>& identifiersToCheck) {
    identifiersToCheck.push_back(calledFun->getFunId());
    CallArgsStmt* calledArgs = calledFun->getCalledArgs().get();
    for (unsigned i = 0; i < calledArgs->getListOfArguments().size(); ++i) {
        // here i check OperationElementStmt s - args of call
        OperationElementStmt* operElem = calledArgs->getListOfArguments()[i].get();
        checkOperationElement(operElem, identifiersToCheck);
    }
}

void Execution::checkReturnStmt(ReturnStmt* returnedStmt, vector<string>& identifiersToCheck) {
    OperationElementStmt* returnedElem = dynamic_cast<OperationElementStmt*>(returnedStmt->getReturnedStatement().get());
    checkOperationElement(returnedElem, identifiersToCheck);
}

void Execution::printTrees() {
    cout << "Classes relations:\n" << endl;
    cout << setw(CLASS_ID_COLUMN) << "Class ID"
        << "|" << setw(INHERITS_COLUMN) << "Inherits"
        << "|" << setw(FRIEND_CLASSES_COLUMN) << "Friend classes"
        << "|" << setw(FRIEND_FUNCTIONS_COLUMN) << "Friend functions" << endl;

    for (unsigned i = 0; i < inhFriendTree.size(); ++i) {
        inhFriendTree[i].printMe();
    }

    cout << endl << "Callings:\n" << endl;

    for (unsigned i = 0; i < callingTree.size(); ++i) {
        callingTree[i].printMe();
    }
}
