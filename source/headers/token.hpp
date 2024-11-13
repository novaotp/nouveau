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
    NULL_KEYWORD,
    RETURN_KEYWORD,
    MUTABLE_KEYWORD,
    IF_KEYWORD,
    ELSE_IF_KEYWORD,
    ELSE_KEYWORD,
    WHILE_KEYWORD,
    FOR_KEYWORD,
    BREAK_KEYWORD,
    CONTINUE_KEYWORD,
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
    QUESTION_MARK,
    END_OF_FILE
};

// ! Doesn't support multi-line tokens
struct TokenMetadata {
    size_t column;
    size_t line;
    size_t length;

    TokenMetadata() : column(0), line(0), length(0) {};
    TokenMetadata(size_t column, size_t line, size_t length) : column(column), line(line), length(length) {};

    inline const NodePosition toStartPosition() const {
        return NodePosition(this->column, this->line);
    }

    inline const NodePosition toEndPosition() const {
        return NodePosition(this->column + this->length, this->line);
    }
};

struct Token {
    TokenType type;
    std::string value;
    TokenMetadata metadata;

    Token() : type(TokenType::INTEGER), value(""), metadata(TokenMetadata()) {};
    Token(TokenType type, std::string value, TokenMetadata metadata) : type(type), value(value), metadata(metadata) {};
};

const std::map<TokenType, std::string> tokenTypeToString = {
    { TokenType::STRING, "String" },
    { TokenType::INTEGER, "Integer" },
    { TokenType::FLOAT, "Float" },
    { TokenType::BOOLEAN, "Boolean" },
    { TokenType::IDENTIFIER, "Identifier" },
    { TokenType::TYPE, "Type" },
    { TokenType::NULL_KEYWORD, "Null" },
    { TokenType::RETURN_KEYWORD, "Return" },
    { TokenType::MUTABLE_KEYWORD, "Mutable" },
    { TokenType::IF_KEYWORD, "If" },
    { TokenType::ELSE_IF_KEYWORD, "Else If" },
    { TokenType::ELSE_KEYWORD, "Else" },
    { TokenType::WHILE_KEYWORD, "While" },
    { TokenType::FOR_KEYWORD, "For" },
    { TokenType::BREAK_KEYWORD, "Break" },
    { TokenType::CONTINUE_KEYWORD, "Continue" },
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
    { TokenType::NOT_OPERATOR, "Not Operator" },
    { TokenType::AND_OPERATOR, "And Operator" },
    { TokenType::OR_OPERATOR, "Or Operator" },
    { TokenType::LEFT_PARENTHESIS, "Left Parenthesis" },
    { TokenType::RIGHT_PARENTHESIS, "Right Parenthesis" },
    { TokenType::LEFT_BRACKET, "Left Bracket" },
    { TokenType::RIGHT_BRACKET, "Right Bracket" },
    { TokenType::LEFT_BRACE, "Left Brace" },
    { TokenType::RIGHT_BRACE, "Right Brace" },
    { TokenType::DOT, "Dot" },
    { TokenType::COMMA, "Comma" },
    { TokenType::COLON, "Colon" },
    { TokenType::SEMI_COLON, "Semicolon" },
    { TokenType::QUESTION_MARK, "Question mark" },
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

inline std::string getTokenTypeString(const TokenType& type) {
    return tokenTypeToString.at(type);
}

#endif // TOKEN_HPP
