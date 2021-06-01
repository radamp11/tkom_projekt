#pragma once
#include <iostream>
#include <iomanip>
#include <variant>
#include "T_Token.h"


enum T_Token {
    //Keywords
    T_KEYWORD_RETURN = 101,
    T_KEYWORD_CLASS = 102,
    T_KEYWORD_FRIEND = 103,
    T_KEYWORD_PUBLIC = 104,
    T_KEYWORD_PRIVATE = 105,

    //Types
    T_TYPE_INT = 201,
    T_TYPE_CHAR = 202,
    T_TYPE_BOOL = 203,
    T_TYPE_VOID = 204,

    //Operators
    T_OPERATOR_DIV = 301,
    T_OPERATOR_MUL = 302,
    T_OPERATOR_ADD = 303,
    T_OPERATOR_SUB = 304,
    T_OPERATOR_EQ = 305,
    T_OPERATOR_CLASS_REF = 306,
    T_OPERATOR_REFERENCE = 307,
    T_OPERATOR_SINGLE_QUOTE = 308,
    T_OPERATOR_COLON = 309,
    T_OPERATOR_SEMICOLON = 310,
    T_OPERATOR_COMMA = 311,

    //BoolOperators
    T_BOOL_OPERATOR_EQ = 401,
    T_BOOL_OPERATOR_LESS = 402,
    T_BOOL_OPERATOR_GR = 403,

    //Brackets
    T_OPEN_BLOCK_BRACKET = 501,
    T_CLOSE_BLOCK_BRACKET = 502,
    T_OPEN_C_BRACKET = 503,
    T_CLOSE_C_BRACKET = 504,

    //Values
    T_VAL_CHARACTER = 601,
    T_VAL_INTEGER = 602,
    T_VAL_TRUE = 603,
    T_VAL_FALSE = 604,

    //Other
    T_IDENTIFIER = 701,
    T_UNKNOWN = 702,
    T_END = 703
};


enum T_TokenCategory {
    C_KEYWORD_TOKEN = 1,
    C_TYPE_TOKEN = 2,
    C_OPERATOR_TOKEN = 3,
    C_BOOL_OPERATOR_TOKEN = 4,
    C_BRACKET_TOKEN = 5,
    C_CONST_VALUES_TOKEN = 6,
    C_OTHER_TOKEN = 7
};


struct Position {
    int lineNum;
    int linePos;

    Position() {
        lineNum = -1;
        linePos = -1;
    }

    Position(int lnNum, int lnPos) {
        lineNum = lnNum;
        linePos = lnPos;
    }
};


class Token {
public:
    T_Token type;
    T_TokenCategory category;
    Position position;
    std::variant<std::string, char, int> value;

    Token(){}
    Token(T_Token t, std::variant<std::string, char, int> val, int lnNum, int lnPos) {
        type = t;
        category = static_cast<T_TokenCategory> (t / 100);
        value = val;
        position = Position(lnNum, lnPos);
    }
    /*
    void printToken() {
        std::cout << std::setw(6) << type << "|" << std::setw(6) << category << "|";
        try {
            int temp1 = std::get<int>(value);
            std::cout << std::setw(15) << temp1;
        }
        catch (std::bad_variant_access const& ex) {}

        try {
            char temp2 = std::get<char>(value);
            std::cout << std::setw(15) << temp2;
        }
        catch (std::bad_variant_access const& ex) {}

        try {
            std::string temp3 = std::get<std::string>(value);
            std::cout << std::setw(15) << temp3;
        }
        catch (std::bad_variant_access const& ex) {}
        std::cout << "|" << std::setw(10) << position.lineNum << "|" << std::setw(10) << position.linePos << std::endl;
    }
    */
};