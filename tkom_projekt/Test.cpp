#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>

#include "LexicalAnalyzer.h"
#include "T_Token.h"


BOOST_AUTO_TEST_SUITE(lexerTests)


BOOST_AUTO_TEST_CASE(classTest)
{
	LexicalAnalyzer lex("parserClassDeclarationTest.txt");
	Token classKey(T_KEYWORD_CLASS, "class", 1, 1);
	Token identifier(T_IDENTIFIER, "SuperKlasa", 1, 7);
	Token semicolon(T_OPERATOR_SEMICOLON, ';', 1, 14);

	BOOST_CHECK(classKey.type == lex.getNextToken()->type);
	BOOST_CHECK(identifier.type == lex.getNextToken()->type);
	BOOST_CHECK(semicolon.type == lex.getNextToken()->type);
}


BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------

#include "Statement.h"
#include "Parser.h"
#include "Program.h"

BOOST_AUTO_TEST_SUITE(parserTests)


BOOST_AUTO_TEST_CASE(classDeclarationTest)
{
	LexicalAnalyzer parserLex("parserClassDeclarationTest.txt");
	Parser parser(&parserLex);
	Program* program = parser.parseProgram();
	ClassDeclarationStmt* stmtToCheck = dynamic_cast<ClassDeclarationStmt*>(program->getFirstStatement());
	//try {
	BOOST_CHECK_EQUAL(stmtToCheck->getClassId(), "SuperKlasa");
	//delete stmtToCheck;
	delete program;
}


BOOST_AUTO_TEST_CASE(funDeclarationTest)
{
	LexicalAnalyzer parserLex("testFunDeclaration.txt");
	Parser parser(&parserLex);
	Program* program = parser.parseProgram();
	if(!program)
		std::cerr << "this program is null" << std::endl;
	FunDeclarationStmt* stmtToCheck = dynamic_cast<FunDeclarationStmt*>(program->getFirstStatement());
	if (!stmtToCheck)
		std::cerr << "this stmt is null" << std::endl;
	//catch (const std::string& e) {
	//	std::cerr << e << std::endl;
	//}

	
	BOOST_CHECK_EQUAL(stmtToCheck->getReturnedType(), MyType::_int);
	BOOST_CHECK_EQUAL(stmtToCheck->getFunId(), "getId");

}

BOOST_AUTO_TEST_CASE(sargumentsStmtTest) {
	LexicalAnalyzer parserLex("testFunDeclaration.txt");
	Parser parser(&parserLex);
	Program* program = parser.parseProgram();
	if (!program)
		std::cerr << "this program is null" << std::endl;

	FunDeclarationStmt* programsMainStmt = dynamic_cast<FunDeclarationStmt*>(program->getFirstStatement());
	if (!programsMainStmt)
		std::cerr << "this stmt is null" << std::endl;

	BOOST_CHECK_EQUAL(programsMainStmt->getArgumentsStmt()->getSignatures().size(), 3);
}

//int notMyId, Klaska obiekcik, Klasa_ & obiekt_
BOOST_AUTO_TEST_CASE(signaturesTest) {
	LexicalAnalyzer parserLex("testFunDeclaration.txt");
	Parser parser(&parserLex);
	Program* program = parser.parseProgram();
	if (!program)
		std::cerr << "this program is null" << std::endl;

	FunDeclarationStmt* programsMainStmt = dynamic_cast<FunDeclarationStmt*>(program->getFirstStatement());
	if (!programsMainStmt)
		std::cerr << "this stmt is null" << std::endl;

	SignatureStmt* paramToCheck1 = dynamic_cast<SignatureStmt*>(programsMainStmt->getArgumentsStmt()->getSignatures()[0]);
	ClassSignatureStmt* paramToCheck2 = dynamic_cast<ClassSignatureStmt*>(programsMainStmt->getArgumentsStmt()->getSignatures()[1]);
	ClassSignatureStmt* paramToCheck3 = dynamic_cast<ClassSignatureStmt*>(programsMainStmt->getArgumentsStmt()->getSignatures()[2]);

	BOOST_CHECK_EQUAL(paramToCheck1->getMyType(), MyType::_int);
	BOOST_CHECK_EQUAL(paramToCheck1->getMyId(), "notMyId");
	BOOST_CHECK_EQUAL(paramToCheck2->getMyClassName(), "Klaska");
	BOOST_CHECK_EQUAL(paramToCheck2->getIsReference(), false);
	BOOST_CHECK_EQUAL(paramToCheck2->getMyId(), "obiekcik");
	BOOST_CHECK_EQUAL(paramToCheck3->getMyClassName(), "Klasa_");
	BOOST_CHECK_EQUAL(paramToCheck3->getIsReference(), true);
	BOOST_CHECK_EQUAL(paramToCheck3->getMyId(), "obiekt_");
}


BOOST_AUTO_TEST_SUITE_END()