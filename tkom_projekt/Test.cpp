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
	//BOOST_TEST(1 == 1);

}


BOOST_AUTO_TEST_SUITE_END()