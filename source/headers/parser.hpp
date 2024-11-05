#ifndef PARSER_HPP
#define PARSER_HPP

#include <variant>
#include <vector>
#include "ast.hpp"
#include "token.hpp"

class Parser {
private:
    std::string sourceCode;
    size_t index = 0;
    std::vector<Token> tokens;

    const Token& getCurrentToken();
    const Token& peekNextToken();
    /// @brief Advances the index by 1.
    /// @return The current token.
    const Token& expectToken();
    /// @brief Checks if the current token matches the given type and advances the index by 1.
    /// 
    ///        Throws a `SyntaxError` if the token types don't match.
    /// @param expectedType The type the current token has to match.
    /// @return The current token.
    /// @exception A `SyntaxError` if the current token doesn't match the given type.
    const Token& expectToken(const TokenType& expected, std::string hint);
    /// @brief Checks if the current token matches any of the given type and advances the index by 1.
    /// 
    ///        Throws a `SyntaxError` if none of the token types match.
    /// @param expectedType The types the current token has to match.
    /// @return The current token.
    /// @exception A `SyntaxError` if the none of the tokens match.
    const Token& expectToken(const std::vector<TokenType>& expected, std::string hint);

    /// @attention
    /// Returns a monostate if it parsed a delimiter such as a ;
    ///             
    /// In those cases, there's no need to handle it.
    std::variant<Statement, Expression, std::monostate> parseStatementOrExpression();
    VariableDeclaration parseVariableDeclaration();
    VariableAssignment parseVariableAssignment();
    IfStatement parseIfStatement();
    WhileStatement parseWhileStatement();
    ForStatement parseForStatement();
    BreakStatement parseBreakStatement();
    ContinueStatement parseContinueStatement();
    ReturnStatement parseReturnStatement();

    Expression parseExpression();
    Expression parseLogicalAndExpression();
    Expression parseLogicalOrExpression();
    Expression parseComparitiveExpression();
    Expression parseAdditiveExpression();
    Expression parseMultiplicativeExpression();
    Expression parseLogicalNotExpression();
    Expression parsePrimitiveExpression();
    Expression parseFunction();
    Expression parseFunctionCall();

public:
    Parser(std::string sourceCode, std::vector<Token> tokens) : sourceCode(sourceCode), tokens(tokens) {};
    ~Parser() {};

    /// @brief Parses an array of tokens into an Abstract Syntax Tree (AST).
    /// @return An Abstract Syntax Tree (AST).
    Program parse();
};

#endif // PARSER_HPP
