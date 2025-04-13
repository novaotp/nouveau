#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "utils.hpp"
#include "parser.hpp"

Parser::Parser(std::string sourceCode, std::vector<Token> tokens) : sourceCode(sourceCode), tokens(tokens) {};
Parser::~Parser() {};

// ! Unsafe
NodeMetadata getExpressionMetadata(const Expression& expr) {
    NodeMetadata metadata;

    std::visit([&metadata](const auto& node) {
        metadata = node.metadata;
    }, expr);

    return metadata;
}

const Token& Parser::getCurrentToken() {
    return this->tokens.at(this->index);
}

const Token& Parser::peekNextToken(size_t n = 1) {
    return this->tokens.at(this->index + n);
}

const Token& Parser::expectToken() {
    return this->tokens.at(this->index++);
}

const Token& Parser::expectToken(const TokenType& expected, std::string hint = "") {
    const Token& currentToken = this->tokens.at(this->index++);

    if (currentToken.type != expected) {
        throw std::runtime_error(
            std::string("\n\tEncountered a syntax error")
            + "\n\n\t" + std::to_string(currentToken.metadata.line) + " | " + splitStringByNewline(this->sourceCode).at(currentToken.metadata.line - 1)
            + "\n\t" + std::string(currentToken.metadata.column + 3, ' ') + RED + std::string(currentToken.metadata.length, '~')
            + GREEN + "\n\n\tExpected : " + getTokenTypeString(expected)
            + RED + "\n\tReceived : " + currentToken.value
            + RESET
            + (hint.size() > 0 ? "\n\n\tHint : " + hint : "")
            + "\n"
        );
    }

    return currentToken;
}

const Token& Parser::expectToken(const std::vector<TokenType>& expected, std::string hint = "") {
    const Token& currentToken = this->tokens.at(this->index++);

    if (std::find(expected.begin(), expected.end(), currentToken.type) == expected.end()) {
        std::string expectedTypes;
        for (const auto& type : expected) {
            expectedTypes += getTokenTypeString(type) + ", ";
        }

        throw std::runtime_error(
            std::string("\n\tEncountered a syntax error")
            + "\n\n\t" + std::to_string(currentToken.metadata.line) + " | " + splitStringByNewline(this->sourceCode).at(currentToken.metadata.line - 1)
            + "\n\t" + std::string(currentToken.metadata.column + 3, ' ') + RED + std::string(currentToken.metadata.length, '~')
            + GREEN + "\n\n\tExpected : " + expectedTypes
            + RED + "\n\tReceived : " + currentToken.value
            + RESET
            + (hint.size() > 0 ? "\n\n\tHint : " + hint : "")
            + "\n"
        );
    }

    return currentToken;
}

BinaryOperator tokenTypeToBinaryOperator(TokenType tokenType) {
    if (tokenType == TokenType::ADDITION_OPERATOR) {
        return BinaryOperator::ADDITION;
    } else if (tokenType == TokenType::SUBTRACTION_OPERATOR) {
        return BinaryOperator::SUBTRACTION;
    } else if (tokenType == TokenType::MULTIPLICATION_OPERATOR) {
        return BinaryOperator::MULTIPLICATION;
    } else if (tokenType == TokenType::DIVISION_OPERATOR) {
        return BinaryOperator::DIVISION;
    } else if (tokenType == TokenType::MODULO_OPERATOR) {
        return BinaryOperator::MODULO;
    } else if (tokenType == TokenType::EQUAL_OPERATOR) {
        return BinaryOperator::EQUAL;
    } else if (tokenType == TokenType::NOT_EQUAL_OPERATOR) {
        return BinaryOperator::NOT_EQUAL;
    } else if (tokenType == TokenType::GREATER_OR_EQUAL_OPERATOR) {
        return BinaryOperator::GREATER_OR_EQUAL;
    } else if (tokenType == TokenType::GREATER_THAN_OPERATOR) {
        return BinaryOperator::GREATER_THAN;
    } else if (tokenType == TokenType::LESS_OR_EQUAL_OPERATOR) {
        return BinaryOperator::LESS_OR_EQUAL;
    } else if (tokenType == TokenType::LESS_THAN_OPERATOR) {
        return BinaryOperator::LESS_THAN;
    } else if (tokenType == TokenType::AND_OPERATOR) {
        return BinaryOperator::AND;
    } else if (tokenType == TokenType::OR_OPERATOR) {
        return BinaryOperator::OR;
    } else {
        throw std::runtime_error("Invalid TokenType to BinaryOperator conversion detected");
    }
}

NodeType Parser::parseType() {
    return this->parsePrimitiveType();
}

NodeType Parser::parsePrimitiveType() {
    Token currentToken = this->expectToken();

    if (currentToken.value == "int") {
        return std::make_shared<IntegerType>(IntegerType());
    } else if (currentToken.value == "float") {
        return std::make_shared<FloatType>(FloatType());
    } else if (currentToken.value == "string") {
        return std::make_shared<StringType>(StringType());
    } else if (currentToken.value == "bool") {
        return std::make_shared<BooleanType>(BooleanType());
    } else {
        throw std::runtime_error(
            std::string("\n\tEncountered a syntax error")
            + "\n\n\t" + std::to_string(currentToken.metadata.line) + " | " + splitStringByNewline(this->sourceCode).at(currentToken.metadata.line - 1)
            + "\n\t" + std::string(currentToken.metadata.column + 3, ' ') + std::string(currentToken.metadata.length, '~')
            + GREEN + "\n\n\tExpected : " + "int, float, string, bool, (<type>)"
            + RED + "\n\tReceived : " + currentToken.value
            + RESET
            + "\n\n\tHint : Did you mean to use a type ?"
            + "\n"
        );
    }
}

Program Parser::parse() {
    Program program;

    while (this->getCurrentToken().type != TokenType::END_OF_FILE) {
        std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(statementOrExpression)) {
            program.body.push_back(
                std::make_shared<Statement>(std::move(std::get<Statement>(statementOrExpression)))
            );
        } else if (std::holds_alternative<Expression>(statementOrExpression)) {
            program.body.push_back(
                std::make_shared<Expression>(std::move(std::get<Expression>(statementOrExpression)))
            );
        }
    }

    return program;
}

std::variant<Statement, Expression, std::monostate> Parser::parseStatementOrExpression() {
    switch (this->getCurrentToken().type) {
    case TokenType::MUTABLE_KEYWORD:
    case TokenType::TYPE:
        return this->parseVariableDeclaration();
    case TokenType::IDENTIFIER:
        if (std::find(
                    tokenTypeAssignmentOperators.begin(),
                    tokenTypeAssignmentOperators.end(),
                    this->peekNextToken().type
                ) != tokenTypeAssignmentOperators.end()) {
            return this->parseVariableAssignment();
        }

        // * Identifier is not a variable assignment, so it must be an expression
        return this->parseExpression();
    case TokenType::SEMI_COLON: {
        this->expectToken(TokenType::SEMI_COLON);
        return std::monostate{};
    }

    // ? Explicitly handle cases to avoid C4061 warnings

    case TokenType::STRING:
    case TokenType::INTEGER:
    case TokenType::FLOAT:
    case TokenType::BOOLEAN:
    case TokenType::ADDITION_OPERATOR:
    case TokenType::SUBTRACTION_OPERATOR:
    case TokenType::MULTIPLICATION_OPERATOR:
    case TokenType::DIVISION_OPERATOR:
    case TokenType::MODULO_OPERATOR:
    case TokenType::ASSIGNMENT_OPERATOR:
    case TokenType::ADDITION_ASSIGNMENT_OPERATOR:
    case TokenType::SUBTRACTION_ASSIGNMENT_OPERATOR:
    case TokenType::MULTIPLICATION_ASSIGNMENT_OPERATOR:
    case TokenType::DIVISION_ASSIGNMENT_OPERATOR:
    case TokenType::MODULO_ASSIGNMENT_OPERATOR:
    case TokenType::EQUAL_OPERATOR:
    case TokenType::NOT_EQUAL_OPERATOR:
    case TokenType::GREATER_THAN_OPERATOR:
    case TokenType::GREATER_OR_EQUAL_OPERATOR:
    case TokenType::LESS_THAN_OPERATOR:
    case TokenType::LESS_OR_EQUAL_OPERATOR:
    case TokenType::AND_OPERATOR:
    case TokenType::OR_OPERATOR:
    case TokenType::EXCLAMATION_MARK:
    case TokenType::LEFT_PARENTHESIS:
    case TokenType::RIGHT_PARENTHESIS:
    case TokenType::END_OF_FILE:
    default:
        return this->parseExpression();
    }
}

VariableDeclaration Parser::parseVariableDeclaration() {
    NodePosition start = this->getCurrentToken().metadata.toStartPosition();

    bool isMutable = false;
    if (this->getCurrentToken().type == TokenType::MUTABLE_KEYWORD) {
        isMutable = true;
        this->expectToken(); // Skip the "mut" token
    }

    NodeType type = this->parseType();
    std::string identifier = this->expectToken(TokenType::IDENTIFIER, "Did you forget to set a name for your variable ? ").value;

    if (this->getCurrentToken().type != TokenType::ASSIGNMENT_OPERATOR) {
        NodePosition end = this->getCurrentToken().metadata.toEndPosition();

        this->expectToken(TokenType::SEMI_COLON, "A variable declaration must end with a ';'. Did you forget it ?"); // Skip the ";" token

        return VariableDeclaration(
                   NodeMetadata(start, end),
                   isMutable,
                   type,
                   identifier,
                   std::nullopt
               );
    }

    this->expectToken(
        TokenType::ASSIGNMENT_OPERATOR,
        "You need to use '=' to assign a value in the initialization. If you don't want to assign a value, end with a ';'."
    ); // Skip the "=" token

    Expression value = this->parseExpression();

    NodePosition end = this->getCurrentToken().metadata.toEndPosition();

    // TODO : Enforce semicolons
    // ? But how to not parse semicolons in constructs like for
    // * Example, for (mut int i = 0; i < 10; i += 1) {}
    // *                                      ~~~~~~~
    // *                                     statement
    if (this->getCurrentToken().type == TokenType::SEMI_COLON) {
        this->expectToken(TokenType::SEMI_COLON, "A variable declaration must end with a ';'. Did you forget it ?"); // Skip the ; token
    }

    return VariableDeclaration(NodeMetadata(start, end), isMutable, type, identifier, std::make_shared<Expression>(std::move(value)));
}

VariableAssignment Parser::parseVariableAssignment() {
    NodePosition start = this->getCurrentToken().metadata.toStartPosition();

    std::string identifier = this->expectToken(TokenType::IDENTIFIER, "Assignment only works on a variable.").value;
    std::string op = this->expectToken(
                         tokenTypeAssignmentOperators,
                         "Use either a simple or a compund assignment operator."
                     ).value; // * Need the operator because of compound assignments
    Expression value = this->parseExpression();

    NodePosition end = this->getCurrentToken().metadata.toEndPosition();

    // TODO : Enforce semicolons
    // ? But how to not parse semicolons in constructs like for
    // * Example, for (mut int i = 0; i < 10; i += 1) {}
    // *                                      ~~~~~~~
    // *                                     statement
    if (this->getCurrentToken().type == TokenType::SEMI_COLON) {
        this->expectToken(TokenType::SEMI_COLON, "A variable declaration must end with a ';'. Did you forget it ?"); // Skip the ";" token
    }

    return VariableAssignment(NodeMetadata(start, end), identifier, op, std::make_shared<Expression>(std::move(value)));
}

Expression Parser::parseExpression() {
    return this->parseLogicalOrExpression();
}

Expression Parser::parseLogicalOrExpression() {
    Expression left = this->parseLogicalAndExpression();

    while (this->getCurrentToken().type == TokenType::OR_OPERATOR) {
        Token op = this->expectToken(TokenType::OR_OPERATOR);
        Expression right = this->parseLogicalAndExpression();
        left = BinaryOperation(
                   NodeMetadata(getExpressionMetadata(left).start, getExpressionMetadata(right).end),
                   std::make_shared<Expression>(std::move(left)),
                   tokenTypeToBinaryOperator(op.type),
                   std::make_shared<Expression>(std::move(right))
               );
    }

    return left;
}

Expression Parser::parseLogicalAndExpression() {
    Expression left = this->parseComparitiveExpression();

    while (this->getCurrentToken().type == TokenType::AND_OPERATOR) {
        Token op = this->expectToken(TokenType::AND_OPERATOR);
        Expression right = this->parseComparitiveExpression();
        left = BinaryOperation(
                   NodeMetadata(getExpressionMetadata(left).start, getExpressionMetadata(right).end),
                   std::make_shared<Expression>(std::move(left)),
                   tokenTypeToBinaryOperator(op.type),
                   std::make_shared<Expression>(std::move(right))
               );
    }

    return left;
}

Expression Parser::parseComparitiveExpression() {
    Expression left = this->parseAdditiveExpression();

    while (this->getCurrentToken().type == TokenType::EQUAL_OPERATOR ||
    this->getCurrentToken().type == TokenType::NOT_EQUAL_OPERATOR ||
    this->getCurrentToken().type == TokenType::GREATER_THAN_OPERATOR ||
    this->getCurrentToken().type == TokenType::GREATER_OR_EQUAL_OPERATOR ||
    this->getCurrentToken().type == TokenType::LESS_THAN_OPERATOR ||
    this->getCurrentToken().type == TokenType::LESS_OR_EQUAL_OPERATOR) {
        Token op = this->expectToken({
            TokenType::EQUAL_OPERATOR,
            TokenType::NOT_EQUAL_OPERATOR,
            TokenType::GREATER_THAN_OPERATOR,
            TokenType::GREATER_OR_EQUAL_OPERATOR,
            TokenType::LESS_THAN_OPERATOR,
            TokenType::LESS_OR_EQUAL_OPERATOR
        });
        Expression right = this->parseAdditiveExpression();
        left = BinaryOperation(
                   NodeMetadata(getExpressionMetadata(left).start, getExpressionMetadata(right).end),
                   std::make_shared<Expression>(std::move(left)),
                   tokenTypeToBinaryOperator(op.type),
                   std::make_shared<Expression>(std::move(right))
               );
    }

    return left;
}

Expression Parser::parseAdditiveExpression() {
    Expression left = this->parseMultiplicativeExpression();

    while (this->getCurrentToken().type == TokenType::ADDITION_OPERATOR ||
    this->getCurrentToken().type == TokenType::SUBTRACTION_OPERATOR) {
        Token op = this->expectToken({ TokenType::ADDITION_OPERATOR, TokenType::SUBTRACTION_OPERATOR });
        Expression right = this->parseMultiplicativeExpression();
        left = BinaryOperation(
                   NodeMetadata(getExpressionMetadata(left).start, getExpressionMetadata(right).end),
                   std::make_shared<Expression>(std::move(left)),
                   tokenTypeToBinaryOperator(op.type),
                   std::make_shared<Expression>(std::move(right))
               );
    }

    return left;
}

Expression Parser::parseMultiplicativeExpression() {
    Expression left = this->parseLogicalNotExpression();

    while (this->getCurrentToken().type == TokenType::MULTIPLICATION_OPERATOR ||
    this->getCurrentToken().type == TokenType::DIVISION_OPERATOR ||
    this->getCurrentToken().type == TokenType::MODULO_OPERATOR) {
        Token op = this->expectToken({ TokenType::MULTIPLICATION_OPERATOR, TokenType::DIVISION_OPERATOR, TokenType::MODULO_OPERATOR });
        Expression right = this->parseLogicalNotExpression();
        left = BinaryOperation(
                   NodeMetadata(getExpressionMetadata(left).start, getExpressionMetadata(right).end),
                   std::make_shared<Expression>(std::move(left)),
                   tokenTypeToBinaryOperator(op.type),
                   std::make_shared<Expression>(std::move(right))
               );
    }

    return left;
}

Expression Parser::parseLogicalNotExpression() {
    if (this->getCurrentToken().type == TokenType::EXCLAMATION_MARK) {
        NodePosition start = this->getCurrentToken().metadata.toStartPosition();

        this->expectToken(TokenType::EXCLAMATION_MARK);
        Expression expression = this->parseLogicalNotExpression();

        return LogicalNotOperation(
                   NodeMetadata(start, getExpressionMetadata(expression).end),
                   std::make_shared<Expression>(std::move(expression))
               );
    }

    return this->parsePrimitiveExpression();
}

Expression Parser::parsePrimitiveExpression() {
    Token currentToken = this->expectToken();

    switch (currentToken.type) {
    case TokenType::STRING:
        return StringLiteral(
                   NodeMetadata(currentToken.metadata.toStartPosition(), currentToken.metadata.toEndPosition()),
                   currentToken.value
               );
    case TokenType::INTEGER:
        return IntLiteral(
                   NodeMetadata(currentToken.metadata.toStartPosition(), currentToken.metadata.toEndPosition()),
                   std::stoi(currentToken.value)
               );
    case TokenType::FLOAT:
        return FloatLiteral(
                   NodeMetadata(currentToken.metadata.toStartPosition(), currentToken.metadata.toEndPosition()),
                   std::stof(currentToken.value)
               );
    case TokenType::BOOLEAN:
        return BooleanLiteral(
                   NodeMetadata(currentToken.metadata.toStartPosition(), currentToken.metadata.toEndPosition()),
        currentToken.value == "true"
               );
    case TokenType::IDENTIFIER:
        return Identifier(
                   NodeMetadata(currentToken.metadata.toStartPosition(), currentToken.metadata.toEndPosition()),
                   currentToken.value
               );
    case TokenType::LEFT_PARENTHESIS: {
        NodePosition start = currentToken.metadata.toStartPosition();
        Expression expression = this->parseExpression();
        NodePosition end = this->getCurrentToken().metadata.toEndPosition();

        this->expectToken(TokenType::RIGHT_PARENTHESIS, "Did you forget to close the parenthesis ?"); // Skip the ")" token

        // * Include the parentheses in the expression's metadata
        std::visit([&start, &end](auto& node) {
            node.metadata.start = start;
            node.metadata.end = end;
        }, expression);

        return expression;
    }

    // ? Explicitly handle cases to avoid C4061 warnings

    case TokenType::ASSIGNMENT_OPERATOR:
    case TokenType::ADDITION_OPERATOR:
    case TokenType::SUBTRACTION_OPERATOR:
    case TokenType::MULTIPLICATION_OPERATOR:
    case TokenType::DIVISION_OPERATOR:
    case TokenType::MODULO_OPERATOR:
    case TokenType::ADDITION_ASSIGNMENT_OPERATOR:
    case TokenType::SUBTRACTION_ASSIGNMENT_OPERATOR:
    case TokenType::MULTIPLICATION_ASSIGNMENT_OPERATOR:
    case TokenType::DIVISION_ASSIGNMENT_OPERATOR:
    case TokenType::MODULO_ASSIGNMENT_OPERATOR:
    case TokenType::TYPE:
    case TokenType::MUTABLE_KEYWORD:
    case TokenType::SEMI_COLON:
    case TokenType::EXCLAMATION_MARK:
    case TokenType::AND_OPERATOR:
    case TokenType::OR_OPERATOR:
    case TokenType::EQUAL_OPERATOR:
    case TokenType::NOT_EQUAL_OPERATOR:
    case TokenType::GREATER_THAN_OPERATOR:
    case TokenType::GREATER_OR_EQUAL_OPERATOR:
    case TokenType::LESS_THAN_OPERATOR:
    case TokenType::LESS_OR_EQUAL_OPERATOR:
    case TokenType::RIGHT_PARENTHESIS:
    case TokenType::END_OF_FILE:
    default:
        throw std::runtime_error("Unsupported token found : " + currentToken.value);
    }
}
