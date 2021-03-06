#pragma once
#include "Exceptions.h"
#include "LexicalAnalyzer.h"
#include "Program.h"
#include "Statement.h"

class Parser {

private:
    // attributes
    Owner owner;

    int classUniqueId = 1;
    Token_ptr currentToken;
    LexicalAnalyzer_ptr lex;
    std::vector<Statement_ptr> tempStatements;
    bool isInClassBlock = false;
    bool isInFunBlock = false;
    bool wasMethodCall = false;
    bool objectCall = false;

    int accessModifier = 0; // 0 - don't care
                            // 1 - public
                            // -1 - private

    // methods
    MyType setType();
    void nextToken();

    ObjectInitStmt_ptr parseObjectInitStmt(string className);

    OperationElementStmt_ptr parseOperationElementStmt();

    ExpressionStmt_ptr parseExpressionStmt(string id);
    //----used to generate expression stmt, which is used in operation elem stmt
    AddOperationStmt_ptr parseAddOperationStmt(OperationElementStmt_ptr firstOperand, bool isAdd);
    MultOperationStmt_ptr parseMultOperationStmt(OperationElementStmt_ptr firstOperand, bool isMult);
    FunCallStmt_ptr parseFunCallStmt(string funId);
    ObjectMethodCallStmt_ptr parseObjectMethodCallStmt(string id, string calledMethod);
    //----end of expression statements

    ReturnStmt_ptr parseReturnStmt();
    AssignStmt_ptr parseAssignStmt(string id);
    InitStmt_ptr parseInitStmt(MyType type, string id);

    SignatureStmt_ptr parseSignatureStmt();
    ClassSignatureStmt_ptr parseClassSignatureStmt();

    BlockStmt_ptr parseBlockStmt();
    ArgumentsStmt_ptr parseArgumentsStmt();
    CallArgsStmt_ptr parseCallArgsStmt();

    ClassDefinitionStmt_ptr parseClassDefinitionStmt(int accessModifier, string classId,
        vector<InheritedClass>& inheritedClasses);
    FunDefinitionStmt_ptr parseFunDefinitionStmt(int accessModifier, MyType returnedType,
        string funId, ArgumentsStmt_ptr argStmt);

    //----uses previous private parse functions and saves new stmt in tempStatements vector
    Statement_ptr parseNewStatement();
    Statement_ptr parseClassStmt(bool isFriend);
    ExpressionStmt_ptr parseObjectCallingStmt(string id);
    bool collectAllStatements();

public:
    Parser(LexicalAnalyzer_ptr lex);
    ~Parser();
    Program_ptr parseProgram();
};

using Parser_ptr = std::unique_ptr<Parser>;
