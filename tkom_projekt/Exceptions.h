#pragma once
#include <iostream>

class MyException : public std::exception {
public:
	virtual void printMessage() {}
	virtual ~MyException() {}
};

class LexicException : public MyException {
public:
	LexicException(const std::string& message) { this->message = message; }
	void printMessage() override { std::cerr << message << std::endl; }

private:
	std::string message;
};

class SyntaxException : public MyException {
public:
	SyntaxException(const std::string& message) { this->message = message; }
	void printMessage() override { std::cerr << message << std::endl; }

private:
	std::string message;
};

class SemanticException : public MyException {
public:
	SemanticException(const std::string& message) { this->message = message; }
	void printMessage() override { std::cerr << message << std::endl; }

private:
	std::string message;
};