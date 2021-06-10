#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>

#include "LexicalAnalyzer.h"
#include "Parser.h"
#include "Program.h"
#include "Statement.h"
#include "T_Token.h"

BOOST_AUTO_TEST_SUITE(lexicTests)

BOOST_AUTO_TEST_CASE(classTest) {
    LexicalAnalyzer_ptr lex = std::make_unique<LexicalAnalyzer>("parserClassDeclarationTest.txt");
    Token_ptr classKey = std::move(lex->getNextToken());   // (T_KEYWORD_CLASS, "class", 1, 1);
    Token_ptr identifier = std::move(lex->getNextToken()); //(T_IDENTIFIER, "SuperKlasa", 1, 7);
    Token_ptr semicolon = std::move(lex->getNextToken());  //(T_OPERATOR_SEMICOLON, ';', 1, 14);

    BOOST_CHECK(classKey->type == T_Token::T_KEYWORD_CLASS);
    BOOST_CHECK(classKey->category == T_TokenCategory::C_KEYWORD_TOKEN);
    BOOST_CHECK(std::get<std::string>(classKey->value) == "class");
    BOOST_CHECK(identifier->type == T_Token::T_IDENTIFIER);
    BOOST_CHECK(identifier->category == T_TokenCategory::C_OTHER_TOKEN);
    BOOST_CHECK(std::get<std::string>(identifier->value) == "SuperKlasa");
    BOOST_CHECK(semicolon->type == T_Token::T_OPERATOR_SEMICOLON);
    BOOST_CHECK(semicolon->category == T_TokenCategory::C_OPERATOR_TOKEN);
    BOOST_CHECK(std::get<char>(semicolon->value) == ';');
}

BOOST_AUTO_TEST_CASE(unknownTokenTest) {
    LexicalAnalyzer_ptr lex = std::make_unique<LexicalAnalyzer>("unknownTokenTest.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(lex));
    Program_ptr program;

    BOOST_CHECK_THROW(program = std::move(parser->parseProgram()), LexicException);
}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------

BOOST_AUTO_TEST_SUITE(syntaxTests)

BOOST_AUTO_TEST_CASE(classDeclarationTest) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("parserClassDeclarationTest.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());
    ClassDeclarationStmt_ptr stmtToCheck(dynamic_cast<ClassDeclarationStmt*>(program->getStatementsList()[0].release()));

    BOOST_CHECK_EQUAL(stmtToCheck->getClassId(), "SuperKlasa");
}

BOOST_AUTO_TEST_CASE(funDeclarationTest) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("testFunDeclaration.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    FunDeclarationStmt_ptr stmtToCheck(dynamic_cast<FunDeclarationStmt*>(program->getStatementsList()[0].release()));

    BOOST_CHECK_EQUAL(stmtToCheck->getReturnedType(), MyType::_int);
    BOOST_CHECK_EQUAL(stmtToCheck->getFunId(), "getId");
}

BOOST_AUTO_TEST_CASE(sargumentsStmtTest) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("testFunDeclaration.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    FunDeclarationStmt_ptr programsMainStmt(
        dynamic_cast<FunDeclarationStmt*>(program->getStatementsList()[0].release()));

    BOOST_CHECK_EQUAL(programsMainStmt->getArgumentsStmt()->getSignatures().size(), 3);
}

// int notMyId, Klaska obiekcik, Klasa_ & obiekt_
BOOST_AUTO_TEST_CASE(signaturesTest) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("testFunDeclaration.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    FunDeclarationStmt_ptr programsMainStmt(
        dynamic_cast<FunDeclarationStmt*>(program->getStatementsList()[0].release()));

    SignatureStmt_ptr paramToCheck1(
        dynamic_cast<SignatureStmt*>(programsMainStmt->getArgumentsStmt()->getSignatures()[0].release()));
    ClassSignatureStmt_ptr paramToCheck2(
        dynamic_cast<ClassSignatureStmt*>(programsMainStmt->getArgumentsStmt()->getSignatures()[1].release()));
    ClassSignatureStmt_ptr paramToCheck3(
        dynamic_cast<ClassSignatureStmt*>(programsMainStmt->getArgumentsStmt()->getSignatures()[2].release()));

    BOOST_CHECK_EQUAL(paramToCheck1->getMyType(), MyType::_int);
    BOOST_CHECK_EQUAL(paramToCheck1->getMyId(), "notMyId");
    BOOST_CHECK_EQUAL(paramToCheck2->getMyClassName(), "Klaska");
    BOOST_CHECK_EQUAL(paramToCheck2->getIsReference(), false);
    BOOST_CHECK_EQUAL(paramToCheck2->getMyId(), "obiekcik");
    BOOST_CHECK_EQUAL(paramToCheck3->getMyClassName(), "Klasa_");
    BOOST_CHECK_EQUAL(paramToCheck3->getIsReference(), true);
    BOOST_CHECK_EQUAL(paramToCheck3->getMyId(), "obiekt_");
}

BOOST_AUTO_TEST_CASE(initStatementTest) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("initStatementTest.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    ClassDefinitionStmt_ptr programsMainStmt(
        dynamic_cast<ClassDefinitionStmt*>(program->getStatementsList()[0].release()));

    BOOST_CHECK_EQUAL(programsMainStmt->getName(), "A");
    BOOST_CHECK(programsMainStmt->getInheritedClasses().empty());

    BlockStmt_ptr classBlock = std::move(programsMainStmt->getDeclarationBlock());

    InitStmt_ptr init1(dynamic_cast<InitStmt*>(classBlock->getStatements()[0].release()));
    InitStmt_ptr init2(dynamic_cast<InitStmt*>(classBlock->getStatements()[1].release()));
    AssignStmt_ptr assign1(dynamic_cast<AssignStmt*>(classBlock->getStatements()[2].release()));
    AssignStmt_ptr assign2(dynamic_cast<AssignStmt*>(classBlock->getStatements()[3].release()));

    BOOST_CHECK_EQUAL(init1->getAccessModifier(), -1);
    BOOST_CHECK_EQUAL(init2->getAccessModifier(), -1);
    BOOST_CHECK_EQUAL(assign1->getAccessModifier(), -1);
    BOOST_CHECK_EQUAL(assign2->getAccessModifier(), 1);
    BOOST_CHECK_EQUAL(init1->getReturnedType(), MyType::_char);
    BOOST_CHECK_EQUAL(init2->getReturnedType(), MyType::_int);
    BOOST_CHECK_EQUAL(init1->getId(), "z");
    BOOST_CHECK_EQUAL(init2->getId(), "a");
    BOOST_CHECK_EQUAL(assign1->getAssigneeId(), "z");
    BOOST_CHECK_EQUAL(assign2->getAssigneeId(), "z");

    OperationElementStmt_ptr oper1 = std::move(init1->getAssignedElement());
    OperationElementStmt_ptr oper2 = std::move(init2->getAssignedElement());
    OperationElementStmt_ptr oper3 = std::move(assign1->getAssignedStatement());
    OperationElementStmt_ptr oper4 = std::move(assign2->getAssignedStatement());

    MyType type1 = std::get<TypeAndValue>(*oper1->getOperationElement()).type;
    char val1 = std::get<char>(std::get<TypeAndValue>(*oper1->getOperationElement()).value);

    BOOST_CHECK_EQUAL(type1, MyType::_char);
    BOOST_CHECK_EQUAL(val1, 'Z');

    ExpressionStmt_ptr expression = std::move(std::get<ExpressionStmt_ptr>(*oper2->getOperationElement()));
    AddOperationStmt_ptr addOper(dynamic_cast<AddOperationStmt*>(expression.release()));

    OperationElementStmt_ptr firstOperand = std::move(addOper->getFirstOperand());
    OperationElementStmt_ptr secondOperand = std::move(addOper->getSecondOperand());

    MyType type2 = std::get<TypeAndValue>(*firstOperand->getOperationElement()).type;
    int val2 = std::get<int>(std::get<TypeAndValue>(*firstOperand->getOperationElement()).value);

    BOOST_CHECK_EQUAL(type2, MyType::_int);
    BOOST_CHECK_EQUAL(val2, 3);

    expression = std::move(std::get<ExpressionStmt_ptr>(*secondOperand->getOperationElement()));
    MultOperationStmt_ptr multOper(dynamic_cast<MultOperationStmt*>(expression.release()));

    firstOperand = std::move(multOper->getFirstOperand());
    secondOperand = std::move(multOper->getSecondOperand());

    MyType type3 = std::get<TypeAndValue>(*firstOperand->getOperationElement()).type;
    int val3 = std::get<int>(std::get<TypeAndValue>(*firstOperand->getOperationElement()).value);
    MyType type4 = std::get<TypeAndValue>(*secondOperand->getOperationElement()).type;
    int val4 = std::get<int>(std::get<TypeAndValue>(*secondOperand->getOperationElement()).value);

    BOOST_CHECK_EQUAL(type3, MyType::_int);
    BOOST_CHECK_EQUAL(val3, 2);
    BOOST_CHECK_EQUAL(type4, MyType::_int);
    BOOST_CHECK_EQUAL(val4, 1);

    MyType type5 = std::get<TypeAndValue>(*oper3->getOperationElement()).type;
    char val5 = std::get<char>(std::get<TypeAndValue>(*oper3->getOperationElement()).value);

    BOOST_CHECK_EQUAL(type5, MyType::_char);
    BOOST_CHECK_EQUAL(val5, 'z');

    expression = std::move(std::get<ExpressionStmt_ptr>(*oper4->getOperationElement()));
    addOper.reset(dynamic_cast<AddOperationStmt*>(expression.release()));

    firstOperand = std::move(addOper->getFirstOperand());
    secondOperand = std::move(addOper->getSecondOperand());

    MyType type6 = std::get<TypeAndValue>(*secondOperand->getOperationElement()).type;
    int val6 = std::get<int>(std::get<TypeAndValue>(*secondOperand->getOperationElement()).value);

    BOOST_CHECK_EQUAL(type6, MyType::_int);
    BOOST_CHECK_EQUAL(val6, 4);

    expression = std::move(std::get<ExpressionStmt_ptr>(*firstOperand->getOperationElement()));
    FunCallStmt_ptr funCall(dynamic_cast<FunCallStmt*>(expression.release()));

    BOOST_CHECK_EQUAL(funCall->getFunId(), "getA");

    CallArgsStmt_ptr calledArgs = std::move(funCall->getCalledArgs());

    BOOST_CHECK_EQUAL(calledArgs->getListOfArguments().size(), 2);

    OperationElementStmt_ptr firstArg(calledArgs->getListOfArguments()[0].release());
    OperationElementStmt_ptr secondArg(calledArgs->getListOfArguments()[1].release());

    string id = std::get<string>(*firstArg->getOperationElement());

    BOOST_CHECK_EQUAL(id, "id");

    expression = std::move(std::get<ExpressionStmt_ptr>(*secondArg->getOperationElement()));
    funCall.reset(dynamic_cast<FunCallStmt*>(expression.release()));

    BOOST_CHECK_EQUAL(funCall->getFunId(), "getB");
    BOOST_CHECK_EQUAL(funCall->getCalledArgs()->getListOfArguments().size(), 0);
}

BOOST_AUTO_TEST_CASE(objectsTest) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("objectsTest.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    FunDefinitionStmt_ptr programsMainStmt(dynamic_cast<FunDefinitionStmt*>(program->getStatementsList()[0].release()));

    BOOST_CHECK_EQUAL(programsMainStmt->getIsFriend(), false);
    BOOST_CHECK_EQUAL(programsMainStmt->getFunId(), "foo");
    BOOST_CHECK_EQUAL(programsMainStmt->getReturnedType(), MyType::_int);

    BlockStmt_ptr funBlock = std::move(programsMainStmt->getBlock());
    ArgumentsStmt_ptr args = std::move(programsMainStmt->getArgs());
    vector<Statement_ptr> stmts = std::move(funBlock->getStatements());
    ObjectInitStmt_ptr objectInit(dynamic_cast<ObjectInitStmt*>(stmts[1].release()));
    ReturnStmt_ptr returnStmt(dynamic_cast<ReturnStmt*>(stmts[2].release()));

    string retStmt = std::get<string>(*std::move(returnStmt->getReturnedStatement()->getOperationElement()));
    BOOST_CHECK_EQUAL(retStmt, "a");

    BOOST_CHECK_EQUAL(objectInit->getClassName(), "Klasa");
    BOOST_CHECK_EQUAL(objectInit->getObjectId(), "drugaKlasa");

    // arguments list is already checked in tests above
}

BOOST_AUTO_TEST_CASE(missingSemicolonTest) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("missingSemicolonTest.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program;

    BOOST_CHECK_THROW(program = std::move(parser->parseProgram()), SyntaxException);
}

BOOST_AUTO_TEST_CASE(invalidAccessModifier) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("invalidAccessModifier.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program;

    BOOST_CHECK_THROW(program = std::move(parser->parseProgram()), SyntaxException);
}

BOOST_AUTO_TEST_CASE(missingCloseBlockBracket) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("missingCloseBlockBracket.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program;

    BOOST_CHECK_THROW(program = std::move(parser->parseProgram()), SyntaxException);
}

BOOST_AUTO_TEST_CASE(missingSingleQuote) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("missingSingleQuote.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program;

    BOOST_CHECK_THROW(program = std::move(parser->parseProgram()), SyntaxException);
}

BOOST_AUTO_TEST_CASE(missingComma1) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("missingComma1.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program;

    BOOST_CHECK_THROW(program = std::move(parser->parseProgram()), SyntaxException);
}

BOOST_AUTO_TEST_CASE(missingComma2) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("missingComma2.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program;

    BOOST_CHECK_THROW(program = std::move(parser->parseProgram()), SyntaxException);
}

BOOST_AUTO_TEST_CASE(missingSemicolon2) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("missingSemicolon2.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program;

    BOOST_CHECK_THROW(program = std::move(parser->parseProgram()), SyntaxException);
}

BOOST_AUTO_TEST_CASE(initOutsideFunOrClass) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("initOutsideFunOrClass.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program;

    BOOST_CHECK_THROW(program = std::move(parser->parseProgram()), SyntaxException);
}

BOOST_AUTO_TEST_CASE(objectInitOutsideFunOrClass) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("objectInitOutsideFunOrClass.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program;

    BOOST_CHECK_THROW(program = std::move(parser->parseProgram()), SyntaxException);
}

BOOST_AUTO_TEST_SUITE_END()

//----------------------------------

BOOST_AUTO_TEST_SUITE(semanticTests)

BOOST_AUTO_TEST_CASE(assignStmtInClass) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("assignStmtInClass.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(objectAttrCallInClass) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("objectAttrCallInClass.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(objectMethodCallInClass) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("objectMethodCallInClass.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(returnStmtInClass) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("returnStmtInClass.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(accessingPrivateAttribute) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("accessingPrivateAttribute.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(friendFunUsesPrivateAttribute) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("friendFunUsesPrivateAttribute.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_NO_THROW(program->run());
}

BOOST_AUTO_TEST_CASE(friendClassUsesPrivateAttribute) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("friendClassUsesPrivateAttribute.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_NO_THROW(program->run());
}

BOOST_AUTO_TEST_CASE(accessingPrivateMethod) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("accessingPrivateMethod.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(instanceOfSameClass) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("instanceOfSameClass.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(objectInitOutOfScope) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("objectInitOutOfScope.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(unknownIdentifier) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("unknownIdentifier.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(unknownMethod) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("unknownMethod.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(wrongNumberOfArguments) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("wrongNumberOfArguments.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(invalidTypes1) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("invalidTypes1.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(invalidTypes2) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("invalidTypes2.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(invalidTypes3) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("invalidTypes3.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(invalidTypes4) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("invalidTypes4.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_CASE(invalidTypes5) {
    LexicalAnalyzer_ptr parserLex = std::make_unique<LexicalAnalyzer>("invalidTypes5.txt");
    Parser_ptr parser = std::make_unique<Parser>(std::move(parserLex));
    Program_ptr program = std::move(parser->parseProgram());

    BOOST_CHECK_THROW(program->run(), SemanticException);
}

BOOST_AUTO_TEST_SUITE_END()