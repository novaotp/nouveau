#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "parser.hpp"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"

const Token& Parser::getCurrentToken() {
    return this->tokens.at(this->index);
}

const Token& Parser::peekNextToken() {
    return this->tokens.at(this->index + 1);
}

const Token& Parser::expectToken() {
    return this->tokens.at(this->index++);
}

std::vector<std::string> splitStringByNewline(const std::string& str) {
    std::vector<std::string> result;
    std::istringstream stream(str);
    std::string line;

    while (std::getline(stream, line)) {
        result.push_back(line);
    }

    return result;
};

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
            + "\n\t" + std::string(currentToken.metadata.column + 3, ' ') + std::string(currentToken.metadata.length, '~')
            + GREEN + "\n\n\tExpected : " + expectedTypes
            + RED + "\n\tReceived : " + currentToken.value
            + RESET
            + (hint.size() > 0 ? "\n\n\tHint : " + hint : "")
            + "\n"
        );
    }

    return currentToken;
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
        case TokenType::IDENTIFIER: {
            if (this->peekNextToken().type == TokenType::LEFT_PARENTHESIS) {
                return this->parseFunctionCall();
            } else {
                return this->parseVariableAssignment();
            }
        }
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
            this->expectToken(TokenType::SEMI_COLON);
            return std::monostate{};
        default:
            return this->parseExpression();
    }
}

VariableDeclaration Parser::parseVariableDeclaration() {
    bool isMutable = this->expectToken(
        { TokenType::CONST_KEYWORD, TokenType::MUTABLE_KEYWORD },
        "A variable declaration must start with either a 'const' or 'mut' keyword.").type == TokenType::MUTABLE_KEYWORD;
    std::string type = this->expectToken(TokenType::TYPE, "Did you forget to define the type of your variable ?").value;
    std::string identifier = this->expectToken(TokenType::IDENTIFIER, "Did you forget to set a name for your variable ? ").value;

    if (this->getCurrentToken().type != TokenType::ASSIGNMENT_OPERATOR) {
        this->expectToken(TokenType::SEMI_COLON, "A variable declaration must end with a ';'. Did you forget it ?"); // Skip the ";" token

        return VariableDeclaration(isMutable, type, identifier, std::nullopt);
    }

    this->expectToken(
        TokenType::ASSIGNMENT_OPERATOR,
        "You need to use '=' to assign a value in the initialization. If you don't want to assign a value, end with a ';'."
    ); // Skip the "=" token

    Expression value = this->parseExpression();

    // TODO : Enforce semicolons
    // ? But how to not parse semicolons in constructs like for
    // * Example, for (mut int i = 0; i < 10; i += 1) {}
    // *                                      ~~~~~~~
    // *                                     statement
    if (this->getCurrentToken().type == TokenType::SEMI_COLON) {
        this->expectToken(TokenType::SEMI_COLON, "A variable declaration must end with a ';'. Did you forget it ?"); // Skip the ; token
    }

    return VariableDeclaration(isMutable, type, identifier, std::make_unique<Expression>(std::move(value)));
}

VariableAssignment Parser::parseVariableAssignment() {
    std::string identifier = this->expectToken(TokenType::IDENTIFIER, "Assignment only works on a variable.").value;
    std::string op = this->expectToken(
        tokenTypeAssignmentOperators,
        "Use either a simple or a compund assignment operator."
    ).value; // * Need the operator because of compound assignments
    Expression value = this->parseExpression();

    // TODO : Enforce semicolons
    // ? But how to not parse semicolons in constructs like for
    // * Example, for (mut int i = 0; i < 10; i += 1) {}
    // *                                      ~~~~~~~
    // *                                     statement
    if (this->getCurrentToken().type == TokenType::SEMI_COLON) {
        this->expectToken(TokenType::SEMI_COLON, "A variable declaration must end with a ';'. Did you forget it ?"); // Skip the ";" token
    }

    return VariableAssignment(identifier, op, std::make_unique<Expression>(std::move(value)));
}

IfStatement Parser::parseIfStatement() {
    this->expectToken(TokenType::IF_KEYWORD, "An 'if' statement must start with an 'if' keyword."); // Skip the "if" token
    this->expectToken(TokenType::LEFT_PARENTHESIS, "The 'if' keyword must be followed by a '('."); // Skip the "(" token

    // Parse the condition expression inside the parentheses
    std::unique_ptr<Expression> condition = std::make_unique<Expression>(this->parseExpression());

    this->expectToken(TokenType::RIGHT_PARENTHESIS, "The 'if' condition must end with a ')'."); // Skip the ")" token
    this->expectToken(TokenType::LEFT_BRACE, "An 'if' condition must be followed by a '{'."); // Skip the "{" token

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

    this->expectToken(TokenType::RIGHT_BRACE, "An 'if' body must be end with a '}'."); // Skip the "}" token

    std::vector<std::pair<std::unique_ptr<Expression>, std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>>>> elseifClauses = {};
    while (this->getCurrentToken().type == TokenType::ELSE_IF_KEYWORD) {
        this->expectToken(TokenType::ELSE_IF_KEYWORD, "An 'else if' statement must start with an 'else if' keyword."); // Skip the "else if" token
        this->expectToken(TokenType::LEFT_PARENTHESIS, "An 'else if' condition must be enclosed between parentheses."); // Skip the "(" token

        std::unique_ptr<Expression> elseifCondition = std::make_unique<Expression>(this->parseExpression());

        this->expectToken(TokenType::RIGHT_PARENTHESIS, "An 'else if' condition must end with a parenthesis."); // Skip the ")" token
        this->expectToken(TokenType::LEFT_BRACE, "An 'else if' body must start with a '{'."); // Skip the "{" token

        // Parse the "else if" block
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

        this->expectToken(TokenType::RIGHT_BRACE, "An 'else if' body must end with a '}'."); // Skip the "}" token

        elseifClauses.push_back(std::make_pair(std::move(elseifCondition), std::move(elseifBlock)));
    }

    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> elseBlock;
    if (this->getCurrentToken().type == TokenType::ELSE_KEYWORD) {
        this->expectToken(TokenType::ELSE_KEYWORD, "An 'else' statement must start with an 'else' keyword."); // Skip the "else" token
        this->expectToken(TokenType::LEFT_BRACE, "An 'else' keyword followed by a '{'."); // Skip the "{" token

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

        this->expectToken(TokenType::RIGHT_BRACE, "An 'else' body must end with a '}'."); // Skip the "}" token
    }

    return IfStatement(
        std::move(condition),
        std::move(thenBlock),
        std::move(elseifClauses),
        std::move(elseBlock)
    );
}

WhileStatement Parser::parseWhileStatement() {
    this->expectToken(TokenType::WHILE_KEYWORD, "A 'while' statement must start with a 'while' keyword."); // Skip "while" token
    this->expectToken(TokenType::LEFT_PARENTHESIS, "A 'while' keyword must be followed by a '('."); // Skip "(" token

    Expression condition = this->parseExpression();

    this->expectToken(TokenType::RIGHT_PARENTHESIS, "A 'while' condition must be end with a ')'."); // Skip ")" token
    this->expectToken(TokenType::LEFT_BRACE, "A 'while' condition must be followed by a '{'."); // Skip "{" token

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

    this->expectToken(TokenType::RIGHT_BRACE, "A 'while' condition must be end with a '}'."); // Skip "}" token

    return WhileStatement(std::make_unique<Expression>(std::move(condition)), std::move(block));
}

ForStatement Parser::parseForStatement() {
    this->expectToken(TokenType::FOR_KEYWORD, "A 'for' statement must start with a 'for' keyword."); // Skip "for" token
    this->expectToken(TokenType::LEFT_PARENTHESIS, "A 'for' statement must be followed by a '('."); // Skip "(" token

    std::optional<std::unique_ptr<Statement>> initialization;
    if (getCurrentToken().type != TokenType::SEMI_COLON) {
        std::variant<Statement, Expression, std::monostate> initializationStatement = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(initializationStatement)) {
            initialization = std::make_unique<Statement>(std::move(std::get<Statement>(initializationStatement)));
        }
    } else {
        this->expectToken(TokenType::SEMI_COLON); // Skip ";" token
    }

    std::optional<std::unique_ptr<Expression>> condition;
    if (this->getCurrentToken().type != TokenType::SEMI_COLON) {
        condition = std::make_unique<Expression>(this->parseExpression());
    }

    this->expectToken(TokenType::SEMI_COLON); // Skip ";" token

    std::optional<std::unique_ptr<Statement>> update;
    if (getCurrentToken().type != TokenType::RIGHT_PARENTHESIS) {
        std::variant<Statement, Expression, std::monostate> updateStmt = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(updateStmt)) {
            update = std::make_unique<Statement>(std::move(std::get<Statement>(updateStmt)));
        }
    }

    this->expectToken(TokenType::RIGHT_PARENTHESIS, "A 'for' statement must be enclosed within parentheses."); // Skip ")" token
    this->expectToken(TokenType::LEFT_BRACE, "A 'for' statement must be followed by a '{'."); // Skip "{" token

    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> block;
    while (getCurrentToken().type != TokenType::RIGHT_BRACE) {
        std::variant<Statement, Expression, std::monostate> element = this->parseStatementOrExpression();
        if (std::holds_alternative<Statement>(element)) {
            block.push_back(std::make_unique<Statement>(std::move(std::get<Statement>(element))));
        } else if (std::holds_alternative<Expression>(element)) {
            block.push_back(std::make_unique<Expression>(std::move(std::get<Expression>(element))));
        }
    }

    this->expectToken(TokenType::RIGHT_BRACE, "A 'for' statement's body must be end with a '}'."); // Skip "}" token

    return ForStatement(std::move(initialization), std::move(condition), std::move(update), std::move(block));
}

BreakStatement Parser::parseBreakStatement() {
    this->expectToken(TokenType::BREAK_KEYWORD); // Skip the "break" token
    this->expectToken(TokenType::SEMI_COLON, "A 'break' keyword must be followed with a ';'."); // Skip the ";" token

    return BreakStatement();
}

ContinueStatement Parser::parseContinueStatement() {
    this->expectToken(TokenType::CONTINUE_KEYWORD); // Skip the "continue" token
    this->expectToken(TokenType::SEMI_COLON, "A 'continue' keyword must be followed with a ';'."); // Skip the ";" token

    return ContinueStatement();
}

ReturnStatement Parser::parseReturnStatement() {
    this->expectToken(TokenType::RETURN_KEYWORD); // Skip the "return" token

    std::optional<std::unique_ptr<Expression>> expression;
    if (this->getCurrentToken().type != TokenType::SEMI_COLON) {
        expression = std::make_unique<Expression>(this->parseExpression());
    }

    this->expectToken(TokenType::SEMI_COLON, "A 'return' keyword/expression must be followed with a ';'."); // Skip the ";" token

    return ReturnStatement(std::move(expression));
}

Expression Parser::parseExpression() {
    return this->parseLogicalOrExpression();
}

Expression Parser::parseLogicalOrExpression() {
    Expression left = this->parseLogicalAndExpression();

    while (this->getCurrentToken().type == TokenType::OR_OPERATOR) {
        Token op = this->expectToken(TokenType::OR_OPERATOR);
        Expression right = this->parseLogicalAndExpression();
        left = BinaryOperation(std::make_unique<Expression>(std::move(left)), op.value, std::make_unique<Expression>(std::move(right)));
    }

    return left;
}

Expression Parser::parseLogicalAndExpression() {
    Expression left = this->parseComparitiveExpression();

    while (this->getCurrentToken().type == TokenType::AND_OPERATOR) {
        Token op = this->expectToken(TokenType::AND_OPERATOR);
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
        Token op = this->expectToken({
            TokenType::EQUAL_OPERATOR,
            TokenType::NOT_EQUAL_OPERATOR,
            TokenType::GREATER_THAN_OPERATOR,
            TokenType::GREATER_OR_EQUAL_OPERATOR,
            TokenType::LESS_THAN_OPERATOR,
            TokenType::LESS_OR_EQUAL_OPERATOR
            });
        Expression right = this->parseAdditiveExpression();
        left = BinaryOperation(std::make_unique<Expression>(std::move(left)), op.value, std::make_unique<Expression>(std::move(right)));
    }

    return left;
}

Expression Parser::parseAdditiveExpression() {
    Expression left = this->parseMultiplicativeExpression();

    while (this->getCurrentToken().type == TokenType::ADDITION_OPERATOR ||
        this->getCurrentToken().type == TokenType::SUBTRACTION_OPERATOR) {
        Token op = this->expectToken({ TokenType::ADDITION_OPERATOR, TokenType::SUBTRACTION_OPERATOR });
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
        Token op = this->expectToken({ TokenType::MULTIPLICATION_OPERATOR, TokenType::DIVISION_OPERATOR, TokenType::MODULO_OPERATOR });
        Expression right = this->parseLogicalNotExpression();
        left = BinaryOperation(std::make_unique<Expression>(std::move(left)), op.value, std::make_unique<Expression>(std::move(right)));
    }

    return left;
}

Expression Parser::parseLogicalNotExpression() {
    if (this->getCurrentToken().type == TokenType::NOT_OPERATOR) {
        this->expectToken(TokenType::NOT_OPERATOR);
        Expression expression = this->parsePrimitiveExpression();

        return LogicalNotOperation(std::make_unique<Expression>(std::move(expression)));
    }

    return this->parsePrimitiveExpression();
}

Expression Parser::parsePrimitiveExpression() {
    Token currentToken = this->expectToken();

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
        case TokenType::IDENTIFIER: {
            if (this->getCurrentToken().type == TokenType::LEFT_PARENTHESIS) {
                this->index -= 1; // * Go back because we skipped the identifier
                return this->parseFunctionCall();
            } else {
                return Identifier(currentToken.value);
            }
        }
        case TokenType::FUNCTION_KEYWORD:
            return this->parseFunction();
        case TokenType::LEFT_PARENTHESIS: {
            Expression expression = this->parseExpression();

            this->expectToken(TokenType::RIGHT_PARENTHESIS, "Did you forget to close the parenthesis ?"); // Skip the ")" token

            return expression;
        }
        case TokenType::LEFT_BRACKET: {
            std::vector<std::unique_ptr<Expression>> expressions = {};

            while (this->getCurrentToken().type != TokenType::RIGHT_BRACKET) {
                expressions.push_back(std::make_unique<Expression>(this->parseExpression()));

                if (this->getCurrentToken().type == TokenType::COMMA) {
                    this->expectToken(TokenType::COMMA);
                }
            }

            this->expectToken(TokenType::RIGHT_BRACKET, "A vector must end with a ']'."); // Skip the "]" token

            return Vector(std::move(expressions));
        }
        default:
            throw std::runtime_error("Unsupported token found : " + currentToken.value);
    }
}

Expression Parser::parseFunction() {
    // * No need to skip the "fn" token because it is
    // * already skipped inside parsePrimitive

    std::string returnType = this->expectToken(TokenType::TYPE, "A function declaration needs a return type.").value;
    std::string identifier = this->expectToken(TokenType::IDENTIFIER, "A function declaration needs a name.").value;

    this->expectToken(TokenType::LEFT_PARENTHESIS, "A function name must be followed by a '('."); // Skip the "(" token

    std::vector<std::unique_ptr<VariableDeclaration>> parameters = {};
    while (this->getCurrentToken().type != TokenType::RIGHT_PARENTHESIS) {
        bool isMutable = this->expectToken(
            { TokenType::CONST_KEYWORD, TokenType::MUTABLE_KEYWORD },
            "A function parameter must start with either a 'const' or 'mut' keyword."
        ).type == TokenType::MUTABLE_KEYWORD;
        std::string type = this->expectToken(TokenType::TYPE, "A function parameter needs a type.").value;
        std::string identifier = this->expectToken(TokenType::IDENTIFIER, "A function parameter needs a name.").value;
        std::optional<std::unique_ptr<Expression>> expression;

        if (this->getCurrentToken().type == TokenType::ASSIGNMENT_OPERATOR) {
            this->expectToken(TokenType::ASSIGNMENT_OPERATOR); // Skip the "=" token
            expression = std::make_unique<Expression>(this->parseExpression());
        }

        parameters.push_back(std::make_unique<VariableDeclaration>(VariableDeclaration(isMutable, type, identifier, std::move(expression))));

        if (this->getCurrentToken().type == TokenType::COMMA) {
            this->expectToken(TokenType::COMMA); // Skip the "," token
        }
    }

    this->expectToken(TokenType::RIGHT_PARENTHESIS, "A function declaration requires to close the parentheses after the parameters."); // Skip the ")" token
    this->expectToken(TokenType::LEFT_BRACE, "A function declaration's body must start with a '{'."); // Skip the "{" token

    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> body = {};
    while (this->getCurrentToken().type != TokenType::RIGHT_BRACE) {
        std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(statementOrExpression)) {
            body.push_back(
                std::make_unique<Statement>(std::move(std::get<Statement>(statementOrExpression)))
            );
        } else if (std::holds_alternative<Expression>(statementOrExpression)) {
            body.push_back(
                std::make_unique<Expression>(std::move(std::get<Expression>(statementOrExpression)))
            );
        }
    }

    this->expectToken(TokenType::RIGHT_BRACE, "A function declaration's body must start with a '}'."); // Skip the "}" token

    return Function(returnType, identifier, std::move(parameters), std::move(body));
}

Expression Parser::parseFunctionCall() {
    std::string identifier = this->expectToken(TokenType::IDENTIFIER, "A function call must have a name.").value;

    this->expectToken(TokenType::LEFT_PARENTHESIS, "Calling a function requires parentheses."); // Skip the "(" token

    std::vector<std::unique_ptr<Expression>> arguments = {};
    while (this->getCurrentToken().type != TokenType::RIGHT_PARENTHESIS) {
        arguments.push_back(std::make_unique<Expression>(this->parseExpression()));

        if (this->getCurrentToken().type == TokenType::COMMA) {
            this->expectToken(TokenType::COMMA);
        }
    }

    this->expectToken(TokenType::RIGHT_PARENTHESIS, "Calling a function must end with parentheses."); // Skip the ")" token

    return FunctionCall(identifier, std::move(arguments));
}
