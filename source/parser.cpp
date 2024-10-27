#include <iostream>
#include <stdexcept>
#include "parser.hpp"

Token Parser::getCurrentToken() {
    return this->tokens.at(this->index);
}

Token Parser::advanceToken() {
    return this->tokens.at(this->index++);
}

Program Parser::parse() {
    Program program;

    while (this->getCurrentToken().type != TokenType::END_OF_FILE) {
        auto statementOrExpression = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(statementOrExpression)) {
            program.body.push_back(
                std::make_unique<Statement>(std::move(std::get<Statement>(statementOrExpression)))
            );
        } else {
            program.body.push_back(
                std::make_unique<Expression>(std::move(std::get<Expression>(statementOrExpression)))
            );
        }
    }

    return program;
}

std::variant<Statement, Expression> Parser::parseStatementOrExpression() {
    switch (this->getCurrentToken().type) {
        case TokenType::CONST_KEYWORD:
        case TokenType::MUTABLE_KEYWORD:
            return this->parseVariableDeclaration();
        default:
            return this->parseExpression();
    }
}

VariableDeclaration Parser::parseVariableDeclaration() {
    bool isMutable = this->advanceToken().type == TokenType::MUTABLE_KEYWORD;
    std::string type = this->advanceToken().value;
    std::string identifier = this->advanceToken().value;

    if (this->getCurrentToken().type != TokenType::ASSIGNMENT_OPERATOR) {
        if (this->getCurrentToken().type == TokenType::SEMI_COLON) {
            this->advanceToken();
        }

        return VariableDeclaration(isMutable, type, identifier, std::nullopt);
    }

    // Skip the = token
    this->advanceToken();

    Expression value = this->parseExpression();

    if (this->getCurrentToken().type == TokenType::SEMI_COLON) {
        this->advanceToken();
    }

    return VariableDeclaration(isMutable, type, identifier, std::make_unique<Expression>(std::move(value)));
}

Expression Parser::parseExpression() {
    return this->parseAdditiveExpression();
}

Expression Parser::parseAdditiveExpression() {
    Expression left = this->parseMultiplicativeExpression();

    while (this->getCurrentToken().type == TokenType::ADDITION_OPERATOR ||
        this->getCurrentToken().type == TokenType::SUBTRACTION_OPERATOR) {
        Token op = this->advanceToken();
        Expression right = this->parseMultiplicativeExpression();
        left = ArithmeticOperation(std::make_unique<Expression>(std::move(left)), op.value, std::make_unique<Expression>(std::move(right)));
    }

    return left;
}

Expression Parser::parseMultiplicativeExpression() {
    Expression left = this->parsePrimitiveExpression();

    while (this->getCurrentToken().type == TokenType::MULTIPLICATION_OPERATOR ||
        this->getCurrentToken().type == TokenType::DIVISION_OPERATOR ||
        this->getCurrentToken().type == TokenType::MODULO_OPERATOR) {
        Token op = this->advanceToken();
        Expression right = this->parsePrimitiveExpression();
        left = ArithmeticOperation(std::make_unique<Expression>(std::move(left)), op.value, std::make_unique<Expression>(std::move(right)));
    }

    return left;
}

Literal Parser::parsePrimitiveExpression() {
    Token currentToken = this->advanceToken();

    switch (currentToken.type) {
        case TokenType::STRING:
            return StringLiteral(currentToken.value);

        case TokenType::INTEGER:
            return IntLiteral(std::stoi(currentToken.value));

        case TokenType::FLOAT:
            return FloatLiteral(std::stof(currentToken.value));

        case TokenType::BOOLEAN:
            return BooleanLiteral(currentToken.value == "true");

        default:
            throw std::runtime_error("Unsupported token found : " + currentToken.value);
    }
}
