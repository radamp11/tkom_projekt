#include "Parser.h"
#include <string>

MyType Parser::setType() {
    MyType type;
    if (currentToken->type == T_TYPE_INT)
        type = MyType::_int;
    else if (currentToken->type == T_TYPE_CHAR)
        type = MyType::_char;
    else if (currentToken->type == T_TYPE_VOID)
        type = MyType::_void;
    else
        std::cerr << "Undefined type in setType function" << endl;

    return type;
}

void Parser::nextToken() {
    currentToken = std::move(lex->getNextToken());
    if (currentToken->type == T_UNKNOWN) {
        throw LexicException("Unknown token at line: " + std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));
    }
}

Parser::Parser(LexicalAnalyzer_ptr lex) {
    this->lex = std::move(lex);
    nextToken();
}

Parser::~Parser() {}

Program_ptr Parser::parseProgram() {
    Program_ptr newProgram(nullptr);
    if (collectAllStatements())
        newProgram.reset(new Program(this->tempStatements));
    return newProgram;
}

bool Parser::collectAllStatements() {
    Statement_ptr newStatement = std::move(parseNewStatement());

    while (newStatement.get() != nullptr) {
        tempStatements.push_back(std::move(newStatement));
        nextToken();
        newStatement = std::move(parseNewStatement());
    }
    // if i have collected all statements from file that means a program
    // was written correctly considering syntax

    return currentToken->type == T_END;
}

Statement_ptr Parser::parseNewStatement() {
    Statement_ptr newStatement(nullptr);
    bool isFriend = false;

    if (currentToken->type == T_END)
        return newStatement;

    if (currentToken->type == T_KEYWORD_FRIEND) {
        isFriend = true;
        nextToken();
    }

    // it is a class declaration or class definition
    if (currentToken->type == T_KEYWORD_CLASS) {
        nextToken();
        newStatement = std::move(parseClassStmt(isFriend));
    }

    // it is a function declaration
    // or function definition
    // or init stmt

    else if (currentToken->category == C_TYPE_TOKEN) {
        SignatureStmt_ptr funSignature = std::move(parseSignatureStmt());

        MyType returnedType = funSignature->getMyType();
        string identifier = funSignature->getMyId();
        nextToken();

        if (currentToken->type == T_OPERATOR_SEMICOLON || currentToken->type == T_OPERATOR_EQ) {
            if (!(isInFunBlock || isInClassBlock))
                throw SyntaxException("NewStatement: Init outside of function or class at line: " +
                    std::to_string(currentToken->position.lineNum) +
                    ", pos: " + std::to_string(currentToken->position.linePos));
            if (isFriend)
                throw "NewStatement: You cannot declare friendship at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos);
            if (currentToken->type == T_OPERATOR_SEMICOLON)
                newStatement.reset(new InitStmt(accessModifier, returnedType, identifier, nullptr));
            else
                newStatement = std::move(parseInitStmt(returnedType, identifier));
            return newStatement;
        }

        else if (currentToken->type != T_OPEN_C_BRACKET)
            throw SyntaxException("NewStatement: No arguments given in function declaration at line: " +
                std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));

        ArgumentsStmt_ptr tmpArgumentsStatement = std::move(parseArgumentsStmt());
        // tokens are moved till token ")"
        nextToken();

        if (currentToken->type == T_OPEN_BLOCK_BRACKET) {
            if (isFriend)
                throw SyntaxException("NewStatement: Cannot both declare friendship and define a function at line: " +
                    std::to_string(currentToken->position.lineNum) +
                    ", pos: " + std::to_string(currentToken->position.linePos));
            // if (isInFunBlock)
            //	throw "NewStatement: Cannot define a function inside another function at line: " +
            //std::to_string(currentToken->position.lineNum)
            //	+ ", pos: " + std::to_string(currentToken->position.linePos);
            newStatement =
                std::move(parseFunDefinitionStmt(accessModifier, returnedType, identifier, std::move(tmpArgumentsStatement)));
        }
        else if (currentToken->type == T_OPERATOR_SEMICOLON) {
            newStatement.reset(new FunDeclarationStmt(accessModifier, owner, returnedType, identifier,
                std::move(tmpArgumentsStatement), isFriend));
        }
        else {
            throw SyntaxException("NewStatement: Missing semicolon in FunDeclaration statement at line: " +
                std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        }
        return newStatement;

    }

    else if (currentToken->type == T_IDENTIFIER) {
        if (isFriend)
            throw SyntaxException(
                "NewStatement: You cannot declare friendship at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        if (!isInFunBlock && !isInClassBlock)
            throw SyntaxException("NewStatement: Unallowed statement outside a class or function at line: " +
                std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        string id = std::get<string>(currentToken->value);
        nextToken();
        if (currentToken->type == T_OPERATOR_CLASS_REF) {
            // object method call or get attribute
            nextToken();
            newStatement = std::move(parseObjectCallingStmt(id));
            if (wasMethodCall)
                nextToken();
            wasMethodCall = false;
            if (currentToken->type != T_OPERATOR_SEMICOLON)
                throw SyntaxException(
                    "NewStatement: missing ';' sign at line: " + std::to_string(currentToken->position.lineNum) +
                    ", pos: " + std::to_string(currentToken->position.linePos));
        }
        else if (currentToken->type == T_OPEN_C_BRACKET) {
            // fun call
            newStatement = std::move(parseFunCallStmt(id));
            nextToken();
            if (currentToken->type != T_OPERATOR_SEMICOLON)
                throw SyntaxException(
                    "NewStatement: missing ';' sign at line: " + std::to_string(currentToken->position.lineNum) +
                    ", pos: " + std::to_string(currentToken->position.linePos));
        }
        else if (currentToken->type == T_OPERATOR_EQ) {
            // assign
            nextToken();
            newStatement = std::move(parseAssignStmt(id));
        }
        else if (currentToken->type == T_IDENTIFIER) {
            // object init statement
            newStatement = std::move(parseObjectInitStmt(id));
        }
    }

    // it is a return stmt
    else if (currentToken->type == T_KEYWORD_RETURN) {
        if (isFriend)
            throw SyntaxException(
                "NewStatement: You cannot declare friendship at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        if (!isInFunBlock)
            throw SyntaxException(
                "NewStatement: Invalid return statement at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        nextToken();
        newStatement = std::move(parseReturnStmt());
        return newStatement;
    }

    // setting private / public flags
    else if (currentToken->type == T_KEYWORD_PRIVATE || currentToken->type == T_KEYWORD_PUBLIC) {
        if (isFriend)
            throw SyntaxException(
                "NewStatement: You cannot declare friendship at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        if (!isInClassBlock)
            throw SyntaxException("NewStatement: Access modifier outside a class block at line: " +
                std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        if (currentToken->type == T_KEYWORD_PRIVATE)
            accessModifier = -1;
        else if (currentToken->type == T_KEYWORD_PUBLIC)
            accessModifier = 1;
        nextToken();
        if (currentToken->type != T_OPERATOR_COLON)
            throw SyntaxException(
                "NewStatement: Missing ':' after access modifier at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        nextToken();
        // recursive
        newStatement = std::move(parseNewStatement());
    }

    return newStatement;
}

InitStmt_ptr Parser::parseInitStmt(MyType type, string id) {
    // current token is not semicolon, so it must be assignment
    if (currentToken->type != T_OPERATOR_EQ)
        throw SyntaxException("InitStmt: Missing semicolon or assignment in init statement at line: " +
            std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));
    nextToken();
    OperationElementStmt_ptr assignedElement = std::move(parseOperationElementStmt());
    InitStmt_ptr newInitStmt = std::make_unique<InitStmt>(accessModifier, type, id, std::move(assignedElement));
    // it can be ended with ',' or ';' without error - i have to check it is only ';'
    if (currentToken->type != T_OPERATOR_SEMICOLON)
        throw SyntaxException("InitStmt: Expected semicolon at line: " + std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));
    return newInitStmt;
}

AssignStmt_ptr Parser::parseAssignStmt(string id) {
    // nextToken();
    AssignStmt_ptr newAssignStmt(new AssignStmt(accessModifier, id, std::move(parseOperationElementStmt())));
    // it can be ended with ',' or ';' without error - i have to check it is only ';'
    if (currentToken->type != T_OPERATOR_SEMICOLON)
        throw SyntaxException("AssignStmt: Expected semicolon at line: " + std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));
    return newAssignStmt;
}

Statement_ptr Parser::parseClassStmt(bool isFriend) {
    Statement_ptr newClassStmt(nullptr);

    // there was a class keyword
    if (currentToken->type != T_IDENTIFIER)
        throw SyntaxException("ClassStmt: Invalid class declaration: missing identifier at line: " +
            std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));
    string classId = std::get<string>(currentToken->value);
    nextToken();
    if (currentToken->type == T_OPERATOR_SEMICOLON) {
        newClassStmt.reset(new ClassDeclarationStmt(accessModifier, classId, isFriend));
        return newClassStmt;
    }

    vector<InheritedClass> inheritedClasses;

    if (currentToken->type == T_OPERATOR_COLON) { // there are classes inherited
        if (isFriend)
            throw SyntaxException("ClassStmt: Cannot both declare friendship and define a class at line: " +
                std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));

        // nextToken();

        do {
            nextToken();
            if (currentToken->type != T_KEYWORD_PUBLIC && currentToken->type != T_KEYWORD_PRIVATE)
                throw SyntaxException("ClassStmt: Missing inheritance access modifier at line: " +
                    std::to_string(currentToken->position.lineNum) +
                    ", pos: " + std::to_string(currentToken->position.linePos));

            InheritedClass newInheritedClass;
            newInheritedClass.access = std::get<string>(currentToken->value);
            nextToken();
            if (currentToken->type != T_IDENTIFIER)
                throw SyntaxException(
                    "ClassStmt: No class identifier provided at line: " + std::to_string(currentToken->position.lineNum) +
                    ", pos: " + std::to_string(currentToken->position.linePos));

            newInheritedClass.className = std::get<string>(currentToken->value);
            inheritedClasses.push_back(newInheritedClass);
            nextToken();
            if (currentToken->type != T_OPERATOR_COMMA && currentToken->type != T_OPEN_BLOCK_BRACKET)
                throw SyntaxException("ClassStmt: Missing comma at line: " + std::to_string(currentToken->position.lineNum) +
                    ", pos: " + std::to_string(currentToken->position.linePos));
        } while (currentToken->type != T_OPEN_BLOCK_BRACKET);
    }

    if (currentToken->type != T_OPEN_BLOCK_BRACKET)
        throw SyntaxException("ClassStmt: Missing '{' sign at line: " + std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));

    newClassStmt = std::move(parseClassDefinitionStmt(accessModifier, classId, inheritedClasses));

    return newClassStmt;
}

SignatureStmt_ptr Parser::parseSignatureStmt() {
    // currently i know that first token is type
    MyType type = setType();
    nextToken();
    if (currentToken->type != T_IDENTIFIER)
        throw SyntaxException(
            "SignatureStmt: Missing identifier at line: " + std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));
    string sigId = std::get<string>(currentToken->value);
    SignatureStmt_ptr newSigStmt = std::make_unique<SignatureStmt>(type, sigId);
    return newSigStmt;
}

ClassSignatureStmt_ptr Parser::parseClassSignatureStmt() {
    // currently i know that first token is class identifier
    string className = std::get<string>(currentToken->value);
    nextToken();
    bool isReference = false;
    if (currentToken->type == T_OPERATOR_REFERENCE) {
        isReference = true;
        nextToken();
    }
    if (currentToken->type != T_IDENTIFIER)
        throw SyntaxException(
            "ClassSignStmt: Missing identifier at line: " + std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));
    string objectId = std::get<string>(currentToken->value);
    ClassSignatureStmt_ptr classDecStmt = std::make_unique<ClassSignatureStmt>(className, isReference, objectId);
    return classDecStmt;
}

ArgumentsStmt_ptr Parser::parseArgumentsStmt() {
    // current token is "("
    nextToken();
    std::vector<SignatureStmt_ptr> signatures;
    SignatureStmt_ptr tmpSignature(nullptr);
    while (currentToken->type != T_CLOSE_C_BRACKET) {
        // it is a normal signature
        if (currentToken->category == C_TYPE_TOKEN) {
            tmpSignature = std::move(parseSignatureStmt());
        }
        // it is a class signature
        else if (currentToken->type == T_IDENTIFIER) {
            tmpSignature = std::move(parseClassSignatureStmt());
        }
        else
            throw SyntaxException("ArgumentsStmt: Cannot parse signature in function arguments at line: " +
                std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));

        if (tmpSignature.get())
            signatures.push_back(std::move(tmpSignature));

        // ended before ")" or ","
        nextToken();
        if (currentToken->type == T_CLOSE_C_BRACKET)
            break;
        if (currentToken->type != T_OPERATOR_COMMA)
            throw SyntaxException(
                "ArgumentsStmt: Missing comma in arguments list at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        nextToken();
    }
    ArgumentsStmt_ptr argsStmt = std::make_unique<ArgumentsStmt>(signatures);
    return argsStmt;
}

ClassDefinitionStmt_ptr Parser::parseClassDefinitionStmt(int access, string classId,
    vector<InheritedClass>& inheritedClasses) {
    // current token is '{'
    bool previousIsInClassBlock = isInClassBlock;
    isInClassBlock = true;
    int previousAccess = accessModifier;
    accessModifier = -1; // default private
    Owner previousOwner = owner;
    owner.ownerName = classId;
    owner.ownerId = classUniqueId;
    int myUniqueId = classUniqueId++;
    ClassDefinitionStmt_ptr newClassDef(nullptr);
    // i am starting from currentToken->type = T_OPEN_BLOCK_BRACKET
    BlockStmt_ptr classBlock = std::move(parseBlockStmt());
    isInClassBlock = previousIsInClassBlock;
    // enden on } sign
    if (!classBlock.get()) {
        // std::cerr << "parse block stmt returned null in class def stmt fun" << endl;
        return newClassDef;
    }
    nextToken();
    if (currentToken->type != T_OPERATOR_SEMICOLON)
        throw SyntaxException("ClassDefStmt: Missing ';' sign in class definition at line: " +
            std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));

    accessModifier = previousAccess;
    owner = previousOwner;
    // access is an accessModifier value form the moment the parseClassDefinitionStmt was called
    newClassDef.reset(new ClassDefinitionStmt(myUniqueId, access, classId, inheritedClasses, std::move(classBlock)));
    return newClassDef;
}

FunDefinitionStmt_ptr Parser::parseFunDefinitionStmt(int access, MyType returnedType, string funId,
    ArgumentsStmt_ptr argStmt) {
    bool previousIsInFunBlock = isInFunBlock;
    isInFunBlock = true;
    FunDefinitionStmt_ptr newFunDef(
        new FunDefinitionStmt(access, owner, returnedType, funId, std::move(argStmt), std::move(parseBlockStmt())));
    isInFunBlock = previousIsInFunBlock;
    return newFunDef;
}

BlockStmt_ptr Parser::parseBlockStmt() {
    // starting at currToken->val = {
    Statement_ptr newBlockStatement(nullptr);
    vector<Statement_ptr> statements;
    nextToken();
    do {
        // nextToken();
        if (currentToken->type == T_CLOSE_BLOCK_BRACKET || currentToken->type == T_END)
            break;
        newBlockStatement = std::move(parseNewStatement());
        if (newBlockStatement.get())
            statements.push_back(std::move(newBlockStatement));
        else
            throw SyntaxException(
                "BlockStmt: Error in parsing a statement at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        nextToken();
        if (currentToken->type == T_OPERATOR_SEMICOLON)
            throw SyntaxException(
                "BlockStmt: Missing '}' bracket at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
    } while (currentToken->type != T_CLOSE_BLOCK_BRACKET);
    BlockStmt_ptr newBlock(new BlockStmt(statements));

    return newBlock;
}

ExpressionStmt_ptr Parser::parseObjectCallingStmt(string id) {
    // current token is called method or attribute id
    if (currentToken->type != T_IDENTIFIER)
        throw SyntaxException("ObjectCallingStmt: Expecting identifier after reference operator at line: " +
            std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));
    string called = std::get<string>(currentToken->value);
    nextToken();
    if (currentToken->type == T_OPEN_C_BRACKET) {
        return std::move(parseObjectMethodCallStmt(id, called));
    }
    else {
        GetObjectAttributeStmt_ptr objectGetArgStmt(new GetObjectAttributeStmt(id, called));
        objectCall = true;
        return objectGetArgStmt;
    }
}

ObjectMethodCallStmt_ptr Parser::parseObjectMethodCallStmt(string id, string calledMethodId) {
    // current token is (
    FunCallStmt_ptr calledMethod = std::move(parseFunCallStmt(calledMethodId));
    ObjectMethodCallStmt_ptr objCallMethodStmt(new ObjectMethodCallStmt(id, std::move(calledMethod)));
    wasMethodCall = true;
    return objCallMethodStmt;
}

ObjectInitStmt_ptr Parser::parseObjectInitStmt(string className) {
    ObjectInitStmt_ptr newObjInitStmt(nullptr);
    // curr token is objects identifier
    if (currentToken->type != T_IDENTIFIER)
        throw SyntaxException(
            "ObjectInitStmt: Missing object's identifier at line: " + std::to_string(currentToken->position.lineNum) +
            ", pos: " + std::to_string(currentToken->position.linePos));
    string objectId = std::get<string>(currentToken->value);
    nextToken();
    FunCallStmt_ptr calledConstructor = std::move(parseFunCallStmt(objectId));
    // ends on ')'
    nextToken();
    newObjInitStmt.reset(new ObjectInitStmt(accessModifier, className, objectId, std::move(calledConstructor)));

    return newObjInitStmt;
}

ReturnStmt_ptr Parser::parseReturnStmt() {
    // current token is first token of returned stmt
    ReturnStmt_ptr retStmt(new ReturnStmt(std::move(parseOperationElementStmt())));
    return retStmt;
}

FunCallStmt_ptr Parser::parseFunCallStmt(string funId) {
    // started on ( token
    FunCallStmt_ptr newFunCall(new FunCallStmt(funId, std::move(parseCallArgsStmt())));
    return newFunCall;
}

CallArgsStmt_ptr Parser::parseCallArgsStmt() {
    // started on ( token
    vector<OperationElementStmt_ptr> calledArguments;

    do {
        nextToken();
        OperationElementStmt_ptr newArg = std::move(parseOperationElementStmt());
        if (newArg.get())
            calledArguments.push_back(std::move(newArg));
        // else
        // std::cerr << "function parseOperationElementStmt returned null in parseFunCallStmt function" << endl;
        // nextToken();
    } while (currentToken->type != T_CLOSE_C_BRACKET && currentToken->type != T_END);

    CallArgsStmt_ptr calledArgs = std::make_unique<CallArgsStmt>(calledArguments);
    return calledArgs;
}

OperationElementStmt_ptr Parser::parseOperationElementStmt() {
    TypeAndValue typeAndValue;
    OperationElementStmt_ptr newOperationElement(nullptr);
    bool wasIdentifier = false;

    // current token is first token of operation element, so check what is it
    if (currentToken->type == T_VAL_INTEGER) {
        typeAndValue.type = _int;
        typeAndValue.value = std::get<int>(currentToken->value);
        newOperationElement.reset(new OperationElementStmt(typeAndValue));
    }
    else if (currentToken->type == T_OPERATOR_SINGLE_QUOTE) {
        nextToken();
        if (currentToken->type != T_VAL_CHARACTER)
            throw SyntaxException(
                "OperationElementStmt: Missing character at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        typeAndValue.type = _char;
        typeAndValue.value = std::get<char>(currentToken->value);
        nextToken();
        if (currentToken->type != T_OPERATOR_SINGLE_QUOTE)
            throw SyntaxException(
                "OperationElementStmt: Missing single quote at line: " + std::to_string(currentToken->position.lineNum) +
                ", pos: " + std::to_string(currentToken->position.linePos));
        newOperationElement.reset(new OperationElementStmt(typeAndValue));
    }
    // it is id or expression
    else if (currentToken->type == T_IDENTIFIER) {
        string id = std::get<string>(currentToken->value);
        nextToken();
        ExpressionStmt_ptr expression = std::move(parseExpressionStmt(id));
        if (expression.get())
            newOperationElement.reset(new OperationElementStmt(std::move(expression)));
        else {
            newOperationElement.reset(new OperationElementStmt(id));
            wasIdentifier = true;
        }
    }
    // if it was only identifier passed, it is already on next token, so don't move it
    // if newOperationElem is empty it means it is an empty arguments list, so don't move it

    if (!wasIdentifier && newOperationElement.get() && !objectCall)
        nextToken();

    objectCall = false;

    if (currentToken->type == T_OPERATOR_ADD || currentToken->type == T_OPERATOR_SUB) {
        OperationElementStmt_ptr firstOperand = std::move(newOperationElement);
        bool isAdd = currentToken->type == T_OPERATOR_ADD;
        nextToken();
        ExpressionStmt_ptr addStmt = std::move(parseAddOperationStmt(std::move(firstOperand), isAdd));
        newOperationElement.reset(new OperationElementStmt(std::move(addStmt)));
    }
    else if (currentToken->type == T_OPERATOR_MUL || currentToken->type == T_OPERATOR_DIV) {
        OperationElementStmt_ptr firstOperand = std::move(newOperationElement);
        bool isMult = currentToken->type == T_OPERATOR_MUL;
        nextToken();
        ExpressionStmt_ptr multStmt = std::move(parseMultOperationStmt(std::move(firstOperand), isMult));
        newOperationElement.reset(new OperationElementStmt(std::move(multStmt)));
    }
    return newOperationElement;
}

// OperationElementStmt_ptr parseOperationElementStmt();

ExpressionStmt_ptr Parser::parseExpressionStmt(string id) {
    // current token is the first token after identifier
    ExpressionStmt_ptr expression(nullptr);

    // object method or attribute calling
    if (currentToken->type == T_OPERATOR_CLASS_REF) {
        nextToken();
        expression = std::move(parseObjectCallingStmt(id));
    }
    // function calling
    else if (currentToken->type == T_OPEN_C_BRACKET) {
        expression = std::move(parseFunCallStmt(id));
    }

    return expression;
}

AddOperationStmt_ptr Parser::parseAddOperationStmt(OperationElementStmt_ptr firstOperand, bool isAdd) {
    AddOperationStmt_ptr newAddStmt(
        new AddOperationStmt(std::move(firstOperand), std::move(parseOperationElementStmt()), isAdd));
    return newAddStmt;
}

MultOperationStmt_ptr Parser::parseMultOperationStmt(OperationElementStmt_ptr firstOperand, bool isMult) {
    MultOperationStmt_ptr newMultStmt(
        new MultOperationStmt(std::move(firstOperand), std::move(parseOperationElementStmt()), isMult));
    return newMultStmt;
}
