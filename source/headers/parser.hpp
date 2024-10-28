#ifndef PARSER_HPP
#define PARSER_HPP

#include <variant>
#include <vector>
#include "ast.hpp"
#include "token.hpp"

class Parser {
    private:
    size_t index = 0;
    std::vector<Token> tokens;

    Token getCurrentToken();
    Token advanceToken();

    std::variant<Statement, Expression> parseStatementOrExpression();
    VariableDeclaration parseVariableDeclaration();
    VariableAssignment parseVariableAssignment();

    Expression parseExpression();
    Expression parseLogicalAndExpression();
    Expression parseLogicalOrExpression();
    Expression parseComparitiveExpression();
    Expression parseAdditiveExpression();
    Expression parseMultiplicativeExpression();
    Expression parseLogicalNotExpression();
    Expression parsePrimitiveExpression();

    public:
    Parser(std::vector<Token> tokens) : tokens(tokens) {};
    ~Parser() {};

    /// @brief Parses an array of tokens into an Abstract Syntax Tree (AST).
    /// @return An Abstract Syntax Tree (AST).
    Program parse();
};

#endif // PARSER_HPP
