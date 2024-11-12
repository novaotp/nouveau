#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include "utils.hpp"
#include "parser.hpp"

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
    // ? Note that functions are not handled here.
    // * Since they can be assigned to variables, they are expressions.
    // * See `parsePrimitive` function for function handling.

    switch (this->getCurrentToken().type) {
        case TokenType::MUTABLE_KEYWORD:
        case TokenType::TYPE:
            // ? This is to handle constant variable declarations and function declarations.
            // * Because the syntax starts the same : <type> <identifier>

            // ! This doesn't handle the case for anonymous functions.
            // ! Nor does it handle variable declarations with big types, such as int[], string? etc.

            if (this->peekNextToken().type == TokenType::IDENTIFIER && this->peekNextToken(2).type == TokenType::LEFT_PARENTHESIS) {
                // Function declaration
                return this->parseExpression();
            }

            // Variable declaration
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
    NodePosition start = this->getCurrentToken().metadata.toStartPosition();

    const Token& currentToken = this->expectToken(
        { TokenType::TYPE, TokenType::MUTABLE_KEYWORD },
        "A function parameter must start with either the 'mut' keyword or a type."
    );
    bool isMutable = currentToken.type == TokenType::MUTABLE_KEYWORD;

    if (currentToken.type == TokenType::TYPE) {
        this->index -= 1;
    }

    std::string type = this->expectToken(TokenType::TYPE, "Did you forget to define the type of your variable ?").value;
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

IfStatement Parser::parseIfStatement() {
    NodePosition start = this->getCurrentToken().metadata.toStartPosition();

    this->expectToken(TokenType::IF_KEYWORD, "An 'if' statement must start with an 'if' keyword."); // Skip the "if" token
    this->expectToken(TokenType::LEFT_PARENTHESIS, "The 'if' keyword must be followed by a '('."); // Skip the "(" token

    // Parse the condition expression inside the parentheses
    std::shared_ptr<Expression> condition = std::make_shared<Expression>(this->parseExpression());

    this->expectToken(TokenType::RIGHT_PARENTHESIS, "The 'if' condition must end with a ')'."); // Skip the ")" token
    this->expectToken(TokenType::LEFT_BRACE, "An 'if' condition must be followed by a '{'."); // Skip the "{" token

    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> thenBlock;
    while (this->getCurrentToken().type != TokenType::RIGHT_BRACE) {
        std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(statementOrExpression)) {
            thenBlock.push_back(
                std::make_shared<Statement>(std::move(std::get<Statement>(statementOrExpression)))
            );
        } else if (std::holds_alternative<Expression>(statementOrExpression)) {
            thenBlock.push_back(
                std::make_shared<Expression>(std::move(std::get<Expression>(statementOrExpression)))
            );
        }
    }

    this->expectToken(TokenType::RIGHT_BRACE, "An 'if' body must be end with a '}'."); // Skip the "}" token

    std::vector<std::pair<std::shared_ptr<Expression>, std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>>>> elseifClauses = {};
    while (this->getCurrentToken().type == TokenType::ELSE_IF_KEYWORD) {
        this->expectToken(TokenType::ELSE_IF_KEYWORD, "An 'else if' statement must start with an 'else if' keyword."); // Skip the "else if" token
        this->expectToken(TokenType::LEFT_PARENTHESIS, "An 'else if' condition must be enclosed between parentheses."); // Skip the "(" token

        std::shared_ptr<Expression> elseifCondition = std::make_shared<Expression>(this->parseExpression());

        this->expectToken(TokenType::RIGHT_PARENTHESIS, "An 'else if' condition must end with a parenthesis."); // Skip the ")" token
        this->expectToken(TokenType::LEFT_BRACE, "An 'else if' body must start with a '{'."); // Skip the "{" token

        // Parse the "else if" block
        std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> elseifBlock;
        while (this->getCurrentToken().type != TokenType::RIGHT_BRACE) {
            std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

            if (std::holds_alternative<Statement>(statementOrExpression)) {
                elseifBlock.push_back(
                    std::make_shared<Statement>(std::move(std::get<Statement>(statementOrExpression)))
                );
            } else if (std::holds_alternative<Expression>(statementOrExpression)) {
                elseifBlock.push_back(
                    std::make_shared<Expression>(std::move(std::get<Expression>(statementOrExpression)))
                );
            }
        }

        this->expectToken(TokenType::RIGHT_BRACE, "An 'else if' body must end with a '}'."); // Skip the "}" token

        elseifClauses.push_back(std::make_pair(std::move(elseifCondition), std::move(elseifBlock)));
    }

    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> elseBlock;
    if (this->getCurrentToken().type == TokenType::ELSE_KEYWORD) {
        this->expectToken(TokenType::ELSE_KEYWORD, "An 'else' statement must start with an 'else' keyword."); // Skip the "else" token
        this->expectToken(TokenType::LEFT_BRACE, "An 'else' keyword followed by a '{'."); // Skip the "{" token

        while (this->getCurrentToken().type != TokenType::RIGHT_BRACE) {
            std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

            if (std::holds_alternative<Statement>(statementOrExpression)) {
                elseBlock.push_back(
                    std::make_shared<Statement>(std::move(std::get<Statement>(statementOrExpression)))
                );
            } else if (std::holds_alternative<Expression>(statementOrExpression)) {
                elseBlock.push_back(
                    std::make_shared<Expression>(std::move(std::get<Expression>(statementOrExpression)))
                );
            }
        }

        this->expectToken(TokenType::RIGHT_BRACE, "An 'else' body must end with a '}'."); // Skip the "}" token
    }

    NodePosition end = this->tokens[this->index - 1].metadata.toEndPosition();

    return IfStatement(
        NodeMetadata(start, end),
        std::move(condition),
        std::move(thenBlock),
        std::move(elseifClauses),
        std::move(elseBlock)
    );
}

WhileStatement Parser::parseWhileStatement() {
    NodePosition start = this->getCurrentToken().metadata.toStartPosition();

    this->expectToken(TokenType::WHILE_KEYWORD, "A 'while' statement must start with a 'while' keyword."); // Skip "while" token
    this->expectToken(TokenType::LEFT_PARENTHESIS, "A 'while' keyword must be followed by a '('."); // Skip "(" token

    Expression condition = this->parseExpression();

    this->expectToken(TokenType::RIGHT_PARENTHESIS, "A 'while' condition must be end with a ')'."); // Skip ")" token
    this->expectToken(TokenType::LEFT_BRACE, "A 'while' condition must be followed by a '{'."); // Skip "{" token

    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> block = {};
    while (this->getCurrentToken().type != TokenType::RIGHT_BRACE) {
        std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(statementOrExpression)) {
            block.push_back(
                std::make_shared<Statement>(std::move(std::get<Statement>(statementOrExpression)))
            );
        } else if (std::holds_alternative<Expression>(statementOrExpression)) {
            block.push_back(
                std::make_shared<Expression>(std::move(std::get<Expression>(statementOrExpression)))
            );
        }
    }

    NodePosition end = this->getCurrentToken().metadata.toEndPosition();

    this->expectToken(TokenType::RIGHT_BRACE, "A 'while' condition must be end with a '}'."); // Skip "}" token

    return WhileStatement(NodeMetadata(start, end), std::make_shared<Expression>(std::move(condition)), std::move(block));
}

ForStatement Parser::parseForStatement() {
    NodePosition start = this->getCurrentToken().metadata.toStartPosition();

    this->expectToken(TokenType::FOR_KEYWORD, "A 'for' statement must start with a 'for' keyword."); // Skip "for" token
    this->expectToken(TokenType::LEFT_PARENTHESIS, "A 'for' statement must be followed by a '('."); // Skip "(" token

    std::optional<std::shared_ptr<Statement>> initialization;
    if (getCurrentToken().type != TokenType::SEMI_COLON) {
        std::variant<Statement, Expression, std::monostate> initializationStatement = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(initializationStatement)) {
            initialization = std::make_shared<Statement>(std::move(std::get<Statement>(initializationStatement)));
        }
    } else {
        this->expectToken(TokenType::SEMI_COLON); // Skip ";" token
    }

    std::optional<std::shared_ptr<Expression>> condition;
    if (this->getCurrentToken().type != TokenType::SEMI_COLON) {
        condition = std::make_shared<Expression>(this->parseExpression());
    }

    this->expectToken(TokenType::SEMI_COLON); // Skip ";" token

    std::optional<std::shared_ptr<Statement>> update;
    if (getCurrentToken().type != TokenType::RIGHT_PARENTHESIS) {
        std::variant<Statement, Expression, std::monostate> updateStmt = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(updateStmt)) {
            update = std::make_shared<Statement>(std::move(std::get<Statement>(updateStmt)));
        }
    }

    this->expectToken(TokenType::RIGHT_PARENTHESIS, "A 'for' statement must be enclosed within parentheses."); // Skip ")" token
    this->expectToken(TokenType::LEFT_BRACE, "A 'for' statement must be followed by a '{'."); // Skip "{" token

    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> block;
    while (getCurrentToken().type != TokenType::RIGHT_BRACE) {
        std::variant<Statement, Expression, std::monostate> element = this->parseStatementOrExpression();
        if (std::holds_alternative<Statement>(element)) {
            block.push_back(std::make_shared<Statement>(std::move(std::get<Statement>(element))));
        } else if (std::holds_alternative<Expression>(element)) {
            block.push_back(std::make_shared<Expression>(std::move(std::get<Expression>(element))));
        }
    }

    NodePosition end = this->getCurrentToken().metadata.toEndPosition();

    this->expectToken(TokenType::RIGHT_BRACE, "A 'for' statement's body must be end with a '}'."); // Skip "}" token

    return ForStatement(NodeMetadata(start, end), std::move(initialization), std::move(condition), std::move(update), std::move(block));
}

BreakStatement Parser::parseBreakStatement() {
    NodePosition start = this->getCurrentToken().metadata.toStartPosition();

    this->expectToken(TokenType::BREAK_KEYWORD); // Skip the "break" token

    NodePosition end = this->getCurrentToken().metadata.toEndPosition();

    this->expectToken(TokenType::SEMI_COLON, "A 'break' keyword must be followed with a ';'."); // Skip the ";" token

    return BreakStatement(NodeMetadata(start, end));
}

ContinueStatement Parser::parseContinueStatement() {
    NodePosition start = this->getCurrentToken().metadata.toStartPosition();

    this->expectToken(TokenType::CONTINUE_KEYWORD); // Skip the "continue" token

    NodePosition end = this->getCurrentToken().metadata.toEndPosition();

    this->expectToken(TokenType::SEMI_COLON, "A 'continue' keyword must be followed with a ';'."); // Skip the ";" token

    return ContinueStatement(NodeMetadata(start, end));
}

ReturnStatement Parser::parseReturnStatement() {
    NodePosition start = this->getCurrentToken().metadata.toStartPosition();

    this->expectToken(TokenType::RETURN_KEYWORD); // Skip the "return" token

    std::optional<std::shared_ptr<Expression>> expression;
    if (this->getCurrentToken().type != TokenType::SEMI_COLON) {
        expression = std::make_shared<Expression>(this->parseExpression());
    }

    NodePosition end = this->getCurrentToken().metadata.toEndPosition();

    this->expectToken(TokenType::SEMI_COLON, "A 'return' keyword/expression must be followed with a ';'."); // Skip the ";" token

    return ReturnStatement(NodeMetadata(start, end), std::move(expression));
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
            op.value,
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
            op.value,
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
            op.value,
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
            op.value,
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
            op.value,
            std::make_shared<Expression>(std::move(right))
        );
    }

    return left;
}

Expression Parser::parseLogicalNotExpression() {
    if (this->getCurrentToken().type == TokenType::NOT_OPERATOR) {
        NodePosition start = this->getCurrentToken().metadata.toStartPosition();

        this->expectToken(TokenType::NOT_OPERATOR);
        Expression expression = this->parsePrimitiveExpression();

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
        case TokenType::NULL_KEYWORD:
            return NullLiteral(
                NodeMetadata(currentToken.metadata.toStartPosition(), currentToken.metadata.toEndPosition())
            );
        case TokenType::IDENTIFIER: {
            if (this->getCurrentToken().type == TokenType::LEFT_PARENTHESIS) {
                this->index -= 1; // * Go back because we skipped the identifier
                return this->parseFunctionCall();
            } else {
                return Identifier(
                    NodeMetadata(currentToken.metadata.toStartPosition(), currentToken.metadata.toEndPosition()),
                    currentToken.value
                );
            }
        }
        case TokenType::TYPE:
            return this->parseFunction();
        case TokenType::LEFT_PARENTHESIS: {
            NodePosition start = currentToken.metadata.toStartPosition();

            Expression expression = this->parseExpression();

            NodePosition end = this->getCurrentToken().metadata.toEndPosition();

            this->expectToken(TokenType::RIGHT_PARENTHESIS, "Did you forget to close the parenthesis ?"); // Skip the ")" token

            // Include the parentheses in the expression's metadata
            std::visit([&start, &end](auto& node) {
                node.metadata.start = start;
                node.metadata.end = end;
            }, expression);

            return expression;
        }
        case TokenType::LEFT_BRACKET: {
            NodePosition start = currentToken.metadata.toStartPosition();

            std::vector<std::shared_ptr<Expression>> expressions = {};

            while (this->getCurrentToken().type != TokenType::RIGHT_BRACKET) {
                expressions.push_back(std::make_shared<Expression>(this->parseExpression()));

                if (this->getCurrentToken().type == TokenType::COMMA) {
                    this->expectToken(TokenType::COMMA);
                }
            }

            NodePosition end = this->getCurrentToken().metadata.toEndPosition();

            this->expectToken(TokenType::RIGHT_BRACKET, "A vector must end with a ']'."); // Skip the "]" token

            return Vector(NodeMetadata(start, end), std::move(expressions));
        }
        default:
            throw std::runtime_error("Unsupported token found : " + currentToken.value);
    }
}

Expression Parser::parseFunction() {
    this->index -= 1; // * Go back because we skipped the type inside `parsePrimitive`

    NodePosition start = this->tokens[this->index].metadata.toStartPosition();

    std::string returnType = this->expectToken(TokenType::TYPE, "A function declaration needs a return type.").value;

    std::optional<std::string> identifier = std::nullopt;
    if (this->getCurrentToken().type == TokenType::IDENTIFIER) {
        identifier = this->expectToken(TokenType::IDENTIFIER, "A function declaration needs a name.").value;
    }

    this->expectToken(TokenType::LEFT_PARENTHESIS, "A function name must be followed by a '('."); // Skip the "(" token

    std::vector<std::shared_ptr<VariableDeclaration>> parameters = {};
    while (this->getCurrentToken().type != TokenType::RIGHT_PARENTHESIS) {
        NodePosition start = this->getCurrentToken().metadata.toStartPosition();

        const Token& currentToken = this->expectToken(
            { TokenType::TYPE, TokenType::MUTABLE_KEYWORD },
            "A function parameter must start with either the 'mut' keyword or a type."
        );
        bool isMutable = currentToken.type == TokenType::MUTABLE_KEYWORD;

        if (currentToken.type == TokenType::TYPE) {
            this->index -= 1;
        }

        std::string type = this->expectToken(TokenType::TYPE, "A function parameter needs a type.").value;
        std::string identifier = this->expectToken(TokenType::IDENTIFIER, "A function parameter needs a name.").value;

        NodePosition end = this->getCurrentToken().metadata.toEndPosition();

        std::optional<std::shared_ptr<Expression>> expression;
        if (this->getCurrentToken().type == TokenType::ASSIGNMENT_OPERATOR) {
            this->expectToken(TokenType::ASSIGNMENT_OPERATOR); // Skip the "=" token

            Expression _expr = this->parseExpression();
            expression = std::make_shared<Expression>(std::move(_expr));

            std::visit([&end](const auto& node) {
                end = node.metadata.end;
            }, _expr);
        }

        parameters.push_back(std::make_shared<VariableDeclaration>(
            VariableDeclaration(
                NodeMetadata(start, end),
                isMutable,
                type,
                identifier,
                std::move(expression)
            )
        ));

        if (this->getCurrentToken().type == TokenType::COMMA) {
            this->expectToken(TokenType::COMMA); // Skip the "," token
        }
    }

    this->expectToken(TokenType::RIGHT_PARENTHESIS, "A function declaration requires to close the parentheses after the parameters."); // Skip the ")" token
    this->expectToken(TokenType::LEFT_BRACE, "A function declaration's body must start with a '{'."); // Skip the "{" token

    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> body = {};
    while (this->getCurrentToken().type != TokenType::RIGHT_BRACE) {
        std::variant<Statement, Expression, std::monostate> statementOrExpression = this->parseStatementOrExpression();

        if (std::holds_alternative<Statement>(statementOrExpression)) {
            body.push_back(
                std::make_shared<Statement>(std::move(std::get<Statement>(statementOrExpression)))
            );
        } else if (std::holds_alternative<Expression>(statementOrExpression)) {
            body.push_back(
                std::make_shared<Expression>(std::move(std::get<Expression>(statementOrExpression)))
            );
        }
    }

    NodePosition end = this->getCurrentToken().metadata.toEndPosition();

    this->expectToken(TokenType::RIGHT_BRACE, "A function declaration's body must start with a '}'."); // Skip the "}" token

    return Function(NodeMetadata(start, end), returnType, identifier, std::move(parameters), std::move(body));
}

Expression Parser::parseFunctionCall() {
    NodePosition start = this->getCurrentToken().metadata.toStartPosition();

    std::string identifier = this->expectToken(TokenType::IDENTIFIER, "A function call must have a name.").value;

    this->expectToken(TokenType::LEFT_PARENTHESIS, "Calling a function requires parentheses."); // Skip the "(" token

    std::vector<std::shared_ptr<Expression>> arguments = {};
    while (this->getCurrentToken().type != TokenType::RIGHT_PARENTHESIS) {
        arguments.push_back(std::make_shared<Expression>(this->parseExpression()));

        if (this->getCurrentToken().type == TokenType::COMMA) {
            this->expectToken(TokenType::COMMA);
        }
    }

    NodePosition end = this->getCurrentToken().metadata.toEndPosition();

    this->expectToken(TokenType::RIGHT_PARENTHESIS, "Calling a function must end with parentheses."); // Skip the ")" token

    return FunctionCall(NodeMetadata(start, end), identifier, std::move(arguments));
}
