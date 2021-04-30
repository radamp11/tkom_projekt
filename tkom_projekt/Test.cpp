#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>

#include "LexicalAnalyzer.h"
#include "TokenType.h"


BOOST_AUTO_TEST_SUITE(Lex)
	std::string filePath = "source.txt";
	LexicalAnalyzer lex(filePath);

/*
BOOST_AUTO_TEST_CASE(CommentCase)
{
	std::string filePath = ""
	LexicalAnalyzer lex(filePath);

	SharedToken token = lekser.getNextToken();

	BOOST_CHECK(token->getType() == TokenTypes::COMMENT);
	BOOST_CHECK_EQUAL(std::get<string>(token->getValue()), "//to jest komentarz ");
}

BOOST_AUTO_TEST_CASE(EOFCase)
{
	istringstream stringStream("   \t \n \t   \n");
	SharedStreamSource stream(new StreamSource(stringStream));
	Lekser lekser(stream);

	SharedToken token = lekser.getNextToken();

	BOOST_CHECK(token->getType() == TokenTypes::END_OF_FILE);
}

BOOST_AUTO_TEST_CASE(NumberCase)
{
	istringstream stringStream("2333413956");
	SharedStreamSource stream(new StreamSource(stringStream));
	Lekser lekser(stream);

	SharedToken token = lekser.getNextToken();

	BOOST_CHECK(token->getType() == TokenTypes::CONST_INT);
	BOOST_CHECK_EQUAL(std::get<int>(token->getValue()), 2333413956);
	BOOST_CHECK(std::get<int>(token->getValue()) != 0);
}

BOOST_AUTO_TEST_CASE(FloatCase)
{
	istringstream stringStream("   34.5521 ");
	SharedStreamSource stream(new StreamSource(stringStream));
	Lekser lekser(stream);
	float testFloat = 34.5521;
	double testDouble = 34.5521;

	SharedToken token = lekser.getNextToken();

	BOOST_CHECK(token->getType() == TokenTypes::CONST_FLOAT);
	BOOST_CHECK_EQUAL(std::get<float>(token->getValue()), testFloat);
	BOOST_CHECK_CLOSE(std::get<float>(token->getValue()), testDouble, 0.01);
}

BOOST_AUTO_TEST_CASE(StringCase)
{
	istringstream stringStream(" \"To sie zgadza\"");
	SharedStreamSource stream(new StreamSource(stringStream));
	Lekser lekser(stream);

	SharedToken token = lekser.getNextToken();

	BOOST_CHECK(token->getType() == TokenTypes::CONST_STRING);
	BOOST_CHECK_EQUAL(std::get<string>(token->getValue()), "To sie zgadza");
	BOOST_CHECK(std::get<string>(token->getValue()) != "To sie nie zgadza");
}

BOOST_AUTO_TEST_CASE(KeyWordCase)
{
	istringstream stringStream("int");
	SharedStreamSource stream(new StreamSource(stringStream));
	Lekser lekser(stream);

	SharedToken token = lekser.getNextToken();

	BOOST_CHECK(token->getType() == TokenTypes::INTEGER);
	BOOST_CHECK_EQUAL(std::get<string>(token->getValue()), "int");
	BOOST_CHECK(std::get<string>(token->getValue()) != "float");
}
*/
BOOST_AUTO_TEST_SUITE_END()
