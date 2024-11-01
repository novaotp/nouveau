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
        std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(statementOrExpression)) {
            program.body.push_back(
                std::make_unique<Statement>(std::move(std::get<Statement>(statementOrExpression)))
            );
        } else if (std::holds_alternative<Expression>(statementOrExpression)) {
            program.body.push_back(
                std::make_unique<Expression>(std::move(std::get<Expression>(statementOrExpression)))
            );
        }
    }

    return program;
}

std::variant<Statement, Expression, std::monostate> Parser::parseStatementOrExpression() {
    switch (this->getCurrentToken().type) {
        case TokenType::CONST_KEYWORD:
        case TokenType::MUTABLE_KEYWORD:
            return this->parseVariableDeclaration();
        case TokenType::IDENTIFIER:
            // TODO : Identifier could be a function
            return this->parseVariableAssignment();
        case TokenType::IF_KEYWORD:
            return this->parseIfStatement();
        case TokenType::WHILE_KEYWORD:
            return this->parseWhileStatement();
        case TokenType::FOR_KEYWORD:
            return this->parseForStatement();
        case TokenType::BREAK_KEYWORD:
            return this->parseBreakStatement();
        case TokenType::CONTINUE_KEYWORD:
            return this->parseContinueStatement();
        case TokenType::RETURN_KEYWORD:
            return this->parseReturnStatement();
        case TokenType::SEMI_COLON:
            this->advanceToken();
            return std::monostate{};
        default:
            return this->parseExpression();
    }
}

VariableDeclaration Parser::parseVariableDeclaration() {
    bool isMutable = this->advanceToken().type == TokenType::MUTABLE_KEYWORD;
    std::string type = this->advanceToken().value;
    std::string identifier = this->advanceToken().value;

    if (this->getCurrentToken().type != TokenType::ASSIGNMENT_OPERATOR) {
        this->advanceToken(); // Skip the ";" token

        return VariableDeclaration(isMutable, type, identifier, std::nullopt);
    }

    this->advanceToken(); // Skip the "=" token

    Expression value = this->parseExpression();

    // TODO : Enforce semicolons
    // ? But how to not parse semicolons in constructs like for
    // * Example, for (mut int i = 0; i < 10; i += 1) {}
    // *                                      ~~~~~~~
    // *                                     statement
    if (this->getCurrentToken().type == TokenType::SEMI_COLON) {
        this->advanceToken(); // Skip the ; token
    }

    return VariableDeclaration(isMutable, type, identifier, std::make_unique<Expression>(std::move(value)));
}

VariableAssignment Parser::parseVariableAssignment() {
    std::string identifier = this->advanceToken().value;
    std::string op = this->advanceToken().value; // * Need the operator because of compound assignments
    Expression value = this->parseExpression();

    // TODO : Enforce semicolons
    // ? But how to not parse semicolons in constructs like for
    // * Example, for (mut int i = 0; i < 10; i += 1) {}
    // *                                      ~~~~~~~
    // *                                     statement
    if (this->getCurrentToken().type == TokenType::SEMI_COLON) {
        this->advanceToken(); // Skip the ";" token
    }

    return VariableAssignment(identifier, op, std::make_unique<Expression>(std::move(value)));
}

IfStatement Parser::parseIfStatement() {
    this->advanceToken(); // Skip the "if" token
    this->advanceToken(); // Skip the "(" token

    // Parse the condition expression inside the parentheses
    std::unique_ptr<Expression> condition = std::make_unique<Expression>(this->parseExpression());

    this->advanceToken(); // Skip the ")" token
    this->advanceToken(); // Skip the "{" token

    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> thenBlock;
    while (this->getCurrentToken().type != TokenType::RIGHT_BRACE) {
        std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(statementOrExpression)) {
            thenBlock.push_back(
                std::make_unique<Statement>(std::move(std::get<Statement>(statementOrExpression)))
            );
        } else if (std::holds_alternative<Expression>(statementOrExpression)) {
            thenBlock.push_back(
                std::make_unique<Expression>(std::move(std::get<Expression>(statementOrExpression)))
            );
        }
    }

    this->advanceToken(); // Skip the "}" token

    std::vector<std::pair<std::unique_ptr<Expression>, std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>>>> elseifClauses = {};
    while (this->getCurrentToken().type == TokenType::ELSE_IF_KEYWORD) {
        this->advanceToken(); // Skip the "elseif" token
        this->advanceToken(); // Skip the "(" token

        std::unique_ptr<Expression> elseifCondition = std::make_unique<Expression>(this->parseExpression());

        this->advanceToken(); // Skip the ")" token
        this->advanceToken(); // Skip the "{" token

        // Parse the "elseif" block
        std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> elseifBlock;
        while (this->getCurrentToken().type != TokenType::RIGHT_BRACE) {
            std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

            if (std::holds_alternative<Statement>(statementOrExpression)) {
                elseifBlock.push_back(
                    std::make_unique<Statement>(std::move(std::get<Statement>(statementOrExpression)))
                );
            } else if (std::holds_alternative<Expression>(statementOrExpression)) {
                elseifBlock.push_back(
                    std::make_unique<Expression>(std::move(std::get<Expression>(statementOrExpression)))
                );
            }
        }

        this->advanceToken(); // Skip the "}" token

        elseifClauses.push_back(std::make_pair(std::move(elseifCondition), std::move(elseifBlock)));
    }

    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> elseBlock;
    if (this->getCurrentToken().type == TokenType::ELSE_KEYWORD) {
        this->advanceToken(); // Skip the "else" token
        this->advanceToken(); // Skip the "{" token

        while (this->getCurrentToken().type != TokenType::RIGHT_BRACE) {
            std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

            if (std::holds_alternative<Statement>(statementOrExpression)) {
                elseBlock.push_back(
                    std::make_unique<Statement>(std::move(std::get<Statement>(statementOrExpression)))
                );
            } else if (std::holds_alternative<Expression>(statementOrExpression)) {
                elseBlock.push_back(
                    std::make_unique<Expression>(std::move(std::get<Expression>(statementOrExpression)))
                );
            }
        }

        this->advanceToken(); // Skip the "}" token
    }

    return IfStatement(
        std::move(condition),
        std::move(thenBlock),
        std::move(elseifClauses),
        std::move(elseBlock)
    );
}

WhileStatement Parser::parseWhileStatement() {
    this->advanceToken(); // Skip "while" token
    this->advanceToken(); // Skip "(" token

    Expression condition = this->parseExpression();

    this->advanceToken(); // Skip ")" token
    this->advanceToken(); // Skip "{" token

    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> block = {};
    while (this->getCurrentToken().type != TokenType::RIGHT_BRACE) {
        std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(statementOrExpression)) {
            block.push_back(
                std::make_unique<Statement>(std::move(std::get<Statement>(statementOrExpression)))
            );
        } else if (std::holds_alternative<Expression>(statementOrExpression)) {
            block.push_back(
                std::make_unique<Expression>(std::move(std::get<Expression>(statementOrExpression)))
            );
        }
    }

    this->advanceToken(); // Skip "}" token

    return WhileStatement(std::make_unique<Expression>(std::move(condition)), std::move(block));
}

ForStatement Parser::parseForStatement() {
    this->advanceToken(); // Skip "for" token
    this->advanceToken(); // Skip "(" token

    std::optional<std::unique_ptr<Statement>> initialization;
    if (getCurrentToken().type != TokenType::SEMI_COLON) {
        std::variant<Statement, Expression, std::monostate> initializationStatement = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(initializationStatement)) {
            initialization = std::make_unique<Statement>(std::move(std::get<Statement>(initializationStatement)));
        }
    } else {
        this->advanceToken(); // Skip ";" token
    }

    std::optional<std::unique_ptr<Expression>> condition;
    if (this->getCurrentToken().type != TokenType::SEMI_COLON) {
        condition = std::make_unique<Expression>(this->parseExpression());
    }

    this->advanceToken(); // Skip ";" token

    std::optional<std::unique_ptr<Statement>> update;
    if (getCurrentToken().type != TokenType::RIGHT_PARENTHESIS) {
        std::variant<Statement, Expression, std::monostate> updateStmt = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(updateStmt)) {
            update = std::make_unique<Statement>(std::move(std::get<Statement>(updateStmt)));
        }
    }

    this->advanceToken(); // Skip ")" token
    this->advanceToken(); // Skip "{" token

    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> block;
    while (getCurrentToken().type != TokenType::RIGHT_BRACE) {
        std::variant<Statement, Expression, std::monostate> element = this->parseStatementOrExpression();
        if (std::holds_alternative<Statement>(element)) {
            block.push_back(std::make_unique<Statement>(std::move(std::get<Statement>(element))));
        } else if (std::holds_alternative<Expression>(element)) {
            block.push_back(std::make_unique<Expression>(std::move(std::get<Expression>(element))));
        }
    }

    this->advanceToken(); // Skip "}" token

    return ForStatement(std::move(initialization), std::move(condition), std::move(update), std::move(block));
}

BreakStatement Parser::parseBreakStatement() {
    this->advanceToken(); // Skip the "break" token
    this->advanceToken(); // Skip the ";" token

    return BreakStatement();
}

ContinueStatement Parser::parseContinueStatement() {
    this->advanceToken(); // Skip the "continue" token
    this->advanceToken(); // Skip the ";" token

    return ContinueStatement();
}

ReturnStatement Parser::parseReturnStatement() {
    this->advanceToken(); // Skip the "return" token

    std::optional<std::unique_ptr<Expression>> expression;
    if (this->getCurrentToken().type != TokenType::SEMI_COLON) {
        expression = std::make_unique<Expression>(this->parseExpression());
    }

    this->advanceToken(); // Skip the ";" token

    return ReturnStatement(std::move(expression));
}

Expression Parser::parseExpression() {
    return this->parseLogicalOrExpression();
}

Expression Parser::parseLogicalOrExpression() {
    Expression left = this->parseLogicalAndExpression();

    while (this->getCurrentToken().type == TokenType::OR_OPERATOR) {
        Token op = this->advanceToken();
        Expression right = this->parseLogicalAndExpression();
        left = BinaryOperation(std::make_unique<Expression>(std::move(left)), op.value, std::make_unique<Expression>(std::move(right)));
    }

    return left;
}

Expression Parser::parseLogicalAndExpression() {
    Expression left = this->parseComparitiveExpression();

    while (this->getCurrentToken().type == TokenType::AND_OPERATOR) {
        Token op = this->advanceToken();
        Expression right = this->parseComparitiveExpression();
        left = BinaryOperation(std::make_unique<Expression>(std::move(left)), op.value, std::make_unique<Expression>(std::move(right)));
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
        Token op = this->advanceToken();
        Expression right = this->parseAdditiveExpression();
        left = BinaryOperation(std::make_unique<Expression>(std::move(left)), op.value, std::make_unique<Expression>(std::move(right)));
    }

    return left;
}

Expression Parser::parseAdditiveExpression() {
    Expression left = this->parseMultiplicativeExpression();

    while (this->getCurrentToken().type == TokenType::ADDITION_OPERATOR ||
        this->getCurrentToken().type == TokenType::SUBTRACTION_OPERATOR) {
        Token op = this->advanceToken();
        Expression right = this->parseMultiplicativeExpression();
        left = BinaryOperation(std::make_unique<Expression>(std::move(left)), op.value, std::make_unique<Expression>(std::move(right)));
    }

    return left;
}

Expression Parser::parseMultiplicativeExpression() {
    Expression left = this->parseLogicalNotExpression();

    while (this->getCurrentToken().type == TokenType::MULTIPLICATION_OPERATOR ||
        this->getCurrentToken().type == TokenType::DIVISION_OPERATOR ||
        this->getCurrentToken().type == TokenType::MODULO_OPERATOR) {
        Token op = this->advanceToken();
        Expression right = this->parseLogicalNotExpression();
        left = BinaryOperation(std::make_unique<Expression>(std::move(left)), op.value, std::make_unique<Expression>(std::move(right)));
    }

    return left;
}

Expression Parser::parseLogicalNotExpression() {
    if (this->getCurrentToken().type == TokenType::NOT_OPERATOR) {
        this->advanceToken();
        Expression expression = this->parsePrimitiveExpression();

        return LogicalNotOperation(std::make_unique<Expression>(std::move(expression)));
    }

    return this->parsePrimitiveExpression();
}

Expression Parser::parsePrimitiveExpression() {
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
        case TokenType::NULL_KEYWORD:
            return NullLiteral();
        case TokenType::IDENTIFIER:
            return Identifier(currentToken.value);
        case TokenType::LEFT_PARENTHESIS: {
            Expression expression = this->parseExpression();

            this->advanceToken(); // Skip the ")" token

            return expression;
        }
        case TokenType::LEFT_BRACKET: {
            std::vector<std::unique_ptr<Expression>> expressions = {};

            while (this->getCurrentToken().type != TokenType::RIGHT_BRACKET) {
                expressions.push_back(std::make_unique<Expression>(this->parseExpression()));

                if (this->getCurrentToken().type == TokenType::COMMA) {
                    this->advanceToken();
                }
            }

            this->advanceToken(); // Skip the "]" token

            return Vector(std::move(expressions));
        }
        default:
            throw std::runtime_error("Unsupported token found : " + currentToken.value);
    }
}
