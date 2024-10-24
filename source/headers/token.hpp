#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum TokenType
{
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    IDENTIFIER,
    TYPE,
    FUNCTION_KEYWORD,
    CONST_KEYWORD,
    MUTABLE_KEYWORD,
    IF_KEYWORD,
    ELSE_IF_KEYWORD,
    ELSE_KEYWORD,
    WHILE_KEYWORD,
    FOR_KEYWORD,
    ADDITION_OPERATOR,
    SUBTRACTION_OPERATOR,
    MULTIPLICATION_OPERATOR,
    DIVISION_OPERATOR,
    ASSIGNMENT_OPERATOR,
    ADDITION_ASSIGNMENT_OPERATOR,
    SUBTRACTION_ASSIGNMENT_OPERATOR,
    MULTIPLICATION_ASSIGNMENT_OPERATOR,
    DIVISION_ASSIGNMENT_OPERATOR,
    EQUAL_OPERATOR,
    NOT_EQUAL_OPERATOR,
    GREATER_THAN_OPERATOR,
    GREATER_OR_EQUAL_OPERATOR,
    LESS_THAN_OPERATOR,
    LESS_OR_EQUAL_OPERATOR,
    NOT_OPERATOR,
    AND_OPERATOR,
    OR_OPERATOR,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    DOT,
    COMMA,
    COLON,
    SEMI_COLON,
    END_OF_FILE
};

struct TokenMetadata
{
    size_t column;
    size_t line;
    size_t length;

    TokenMetadata() : column(0), line(0), length(0) {}
    TokenMetadata(size_t column, size_t line, size_t length) : column(column), line(line), length(length) {}
};

struct Token
{
    TokenType type;
    std::string value;
    TokenMetadata metadata;

    Token() : type(TokenType::INTEGER), value(""), metadata(TokenMetadata()) {}
    Token(TokenType type, std::string value, TokenMetadata metadata) : type(type), value(value), metadata(metadata) {}
};

#endif // TOKEN_HPP
