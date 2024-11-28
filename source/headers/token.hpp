#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <map>
#include <vector>
#include "ast.hpp"

enum TokenType {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    IDENTIFIER,
    TYPE,
    MUTABLE_KEYWORD,
    ADDITION_OPERATOR,
    SUBTRACTION_OPERATOR,
    MULTIPLICATION_OPERATOR,
    DIVISION_OPERATOR,
    MODULO_OPERATOR,
    ASSIGNMENT_OPERATOR,
    ADDITION_ASSIGNMENT_OPERATOR,
    SUBTRACTION_ASSIGNMENT_OPERATOR,
    MULTIPLICATION_ASSIGNMENT_OPERATOR,
    DIVISION_ASSIGNMENT_OPERATOR,
    MODULO_ASSIGNMENT_OPERATOR,
    EQUAL_OPERATOR,
    NOT_EQUAL_OPERATOR,
    GREATER_THAN_OPERATOR,
    GREATER_OR_EQUAL_OPERATOR,
    LESS_THAN_OPERATOR,
    LESS_OR_EQUAL_OPERATOR,
    AND_OPERATOR,
    OR_OPERATOR,
    SEMI_COLON,
    EXCLAMATION_MARK,
    END_OF_FILE
};

// ! Doesn't support multi-line tokens
struct TokenMetadata {
    size_t column;
    size_t line;
    size_t length;

    TokenMetadata();
    TokenMetadata(size_t column, size_t line, size_t length);

    const NodePosition toStartPosition() const;
    const NodePosition toEndPosition() const;
};

struct Token {
    TokenType type;
    std::string value;
    TokenMetadata metadata;

    Token();
    Token(TokenType type, std::string value, TokenMetadata metadata);
};

const std::map<TokenType, std::string> tokenTypeToString = {
    { TokenType::STRING, "String" },
    { TokenType::INTEGER, "Integer" },
    { TokenType::FLOAT, "Float" },
    { TokenType::BOOLEAN, "Boolean" },
    { TokenType::IDENTIFIER, "Identifier" },
    { TokenType::TYPE, "Type" },
    { TokenType::MUTABLE_KEYWORD, "Mutable" },
    { TokenType::ADDITION_OPERATOR, "Addition Operator" },
    { TokenType::SUBTRACTION_OPERATOR, "Subtraction Operator" },
    { TokenType::MULTIPLICATION_OPERATOR, "Multiplication Operator" },
    { TokenType::DIVISION_OPERATOR, "Division Operator" },
    { TokenType::MODULO_OPERATOR, "Modulo Operator" },
    { TokenType::ASSIGNMENT_OPERATOR, "Assignment Operator" },
    { TokenType::ADDITION_ASSIGNMENT_OPERATOR, "Addition Assignment Operator" },
    { TokenType::SUBTRACTION_ASSIGNMENT_OPERATOR, "Subtraction Assignment Operator" },
    { TokenType::MULTIPLICATION_ASSIGNMENT_OPERATOR, "Multiplication Assignment Operator" },
    { TokenType::DIVISION_ASSIGNMENT_OPERATOR, "Division Assignment Operator" },
    { TokenType::MODULO_ASSIGNMENT_OPERATOR, "Modulo Assignment Operator" },
    { TokenType::EQUAL_OPERATOR, "Equal Operator" },
    { TokenType::NOT_EQUAL_OPERATOR, "Not Equal Operator" },
    { TokenType::GREATER_THAN_OPERATOR, "Greater Than Operator" },
    { TokenType::GREATER_OR_EQUAL_OPERATOR, "Greater or Equal Operator" },
    { TokenType::LESS_THAN_OPERATOR, "Less Than Operator" },
    { TokenType::LESS_OR_EQUAL_OPERATOR, "Less or Equal Operator" },
    { TokenType::AND_OPERATOR, "And Operator" },
    { TokenType::OR_OPERATOR, "Or Operator" },
    { TokenType::SEMI_COLON, "Semicolon" },
    { TokenType::EXCLAMATION_MARK, "Exclamation mark" },
    { TokenType::END_OF_FILE, "End of File" }
};

const std::vector<TokenType> tokenTypeAssignmentOperators = {
    TokenType::ASSIGNMENT_OPERATOR,
    TokenType::ADDITION_ASSIGNMENT_OPERATOR,
    TokenType::SUBTRACTION_ASSIGNMENT_OPERATOR,
    TokenType::MULTIPLICATION_ASSIGNMENT_OPERATOR,
    TokenType::DIVISION_ASSIGNMENT_OPERATOR,
    TokenType::MODULO_ASSIGNMENT_OPERATOR
};

std::string getTokenTypeString(const TokenType& type);

#endif // TOKEN_HPP
