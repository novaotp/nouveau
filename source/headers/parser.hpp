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

    /// @attention
    /// Returns a monostate if it parsed a delimiter such as a ;
    ///             
    /// In those cases, there's no need to handle it.
    std::variant<Statement, Expression, std::monostate> parseStatementOrExpression();
    VariableDeclaration parseVariableDeclaration();
    VariableAssignment parseVariableAssignment();
    IfStatement parseIfStatement();
    WhileStatement parseWhileStatement();

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
