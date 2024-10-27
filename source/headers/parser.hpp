#ifndef PARSER_HPP
#define PARSER_HPP

#include <variant>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <optional>
#include "token.hpp"

enum NodeKind {
    PROGRAM,
    VARIABLE_DECLARATION,
    VARIABLE_ASSIGNMENT,
    ARITHMETIC_OPERATION,
    STRING_LITERAL,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    BOOLEAN_LITERAL,
    NULL_LITERAL
};

struct Node {
    NodeKind kind;

    Node(NodeKind kind) : kind(kind) {}
};

struct StringLiteral : public Node {
    std::string value;

    StringLiteral() : Node(NodeKind::STRING_LITERAL), value("") {};
    StringLiteral(std::string value) : Node(NodeKind::STRING_LITERAL), value(value) {};
};

struct IntLiteral : public Node {
    int value;

    IntLiteral() : Node(NodeKind::INTEGER_LITERAL), value(0) {};
    IntLiteral(int value) : Node(NodeKind::INTEGER_LITERAL), value(value) {};
};

struct FloatLiteral : public Node {
    float value;

    FloatLiteral() : Node(NodeKind::FLOAT_LITERAL), value(0.0) {};
    FloatLiteral(float value) : Node(NodeKind::FLOAT_LITERAL), value(value) {};
};

struct BooleanLiteral : public Node {
    bool value;

    BooleanLiteral() : Node(NodeKind::BOOLEAN_LITERAL), value(false) {};
    BooleanLiteral(bool value) : Node(NodeKind::BOOLEAN_LITERAL), value(value) {};
};

struct NullLiteral : public Node {
    NullLiteral() : Node(NodeKind::NULL_LITERAL) {};
};

struct ArithmeticOperation;

using Literal = std::variant<StringLiteral, IntLiteral, FloatLiteral, BooleanLiteral, NullLiteral>;
using Expression = std::variant<ArithmeticOperation, Literal>;

struct ArithmeticOperation : public Node {
    std::unique_ptr<Expression> lhs;
    std::string op;
    std::unique_ptr<Expression> rhs;

    ArithmeticOperation(std::unique_ptr<Expression> left, const std::string& operation, std::unique_ptr<Expression> right)
        : Node(NodeKind::ARITHMETIC_OPERATION), lhs(std::move(left)), op(operation), rhs(std::move(right)) {}
};

struct VariableDeclaration : public Node {
    bool isMutable;
    std::string type;
    std::string identifier;
    std::optional<std::unique_ptr<Expression>> value;

    VariableDeclaration(bool isMutable, const std::string& type, const std::string& identifier, std::optional<std::unique_ptr<Expression>> value = std::nullopt)
        : Node(NodeKind::VARIABLE_DECLARATION), isMutable(isMutable), type(type), identifier(identifier), value(std::move(value)) {}
};

struct VariableAssignment : public Node {
    std::string identifier;
    std::optional<std::unique_ptr<Expression>> value;

    VariableAssignment(const std::string& identifier, std::optional<std::unique_ptr<Expression>> value = std::nullopt)
        : Node(NodeKind::VARIABLE_ASSIGNMENT), identifier(identifier), value(std::move(value)) {}
};

using Statement = std::variant<VariableDeclaration, VariableAssignment>;

struct Program : public Node {
    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> body;

    Program() : Node(NodeKind::PROGRAM), body{} {};
};

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
    Expression parseMultiplicativeExpression();
    Expression parseAdditiveExpression();
    Expression parsePrimitiveExpression();

    public:
    Parser(std::vector<Token> tokens) : tokens(tokens) {};
    ~Parser() {};

    /// @brief Parses an array of tokens into an Abstract Syntax Tree (AST).
    /// @return An Abstract Syntax Tree (AST).
    Program parse();
};

#endif // PARSER_HPP
