#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum TokenType
{
    STRING,
    INTEGER,
    FLOAT,
    ADDITION_OPERATOR,
    SUBTRACTION_OPERATOR,
    MULTIPLICATION_OPERATOR,
    DIVISION_OPERATOR,
    SEMI_COLUMN,
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
