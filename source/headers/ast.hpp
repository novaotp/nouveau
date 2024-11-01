#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <optional>

struct StringLiteral {
    std::string value;

    StringLiteral() : value("") {};
    StringLiteral(std::string value) : value(value) {};
};

struct IntLiteral {
    int value;

    IntLiteral() : value(0) {};
    IntLiteral(int value) : value(value) {};
};

struct FloatLiteral {
    float value;

    FloatLiteral() : value(0.0) {};
    FloatLiteral(float value) : value(value) {};
};

struct BooleanLiteral {
    bool value;

    BooleanLiteral() : value(false) {};
    BooleanLiteral(bool value) : value(value) {};
};

struct NullLiteral {
    NullLiteral() {};
};

struct Identifier {
    std::string name;

    Identifier(std::string name) : name(name) {}
};

struct LogicalNotOperation;
struct BinaryOperation;
struct Vector;

using Expression = std::variant<BinaryOperation, LogicalNotOperation, Vector, Identifier, StringLiteral, IntLiteral, FloatLiteral, BooleanLiteral, NullLiteral>;

struct Vector {
    std::vector<std::unique_ptr<Expression>> values;

    Vector(std::vector<std::unique_ptr<Expression>> values) : values(std::move(values)) {}
};

struct LogicalNotOperation {
    std::unique_ptr<Expression> expression;

    LogicalNotOperation(std::unique_ptr<Expression> expression) : expression(std::move(expression)) {};
};

struct BinaryOperation {
    std::unique_ptr<Expression> lhs;
    std::string op;
    std::unique_ptr<Expression> rhs;

    BinaryOperation(std::unique_ptr<Expression> left, const std::string& op, std::unique_ptr<Expression> right)
        : lhs(std::move(left)), op(op), rhs(std::move(right)) {}
};

struct VariableDeclaration {
    bool isMutable;
    std::string type;
    std::string identifier;
    std::optional<std::unique_ptr<Expression>> value;

    VariableDeclaration(bool isMutable, const std::string& type, const std::string& identifier, std::optional<std::unique_ptr<Expression>> value = std::nullopt)
        : isMutable(isMutable), type(type), identifier(identifier), value(std::move(value)) {}
};

struct VariableAssignment {
    std::string identifier;
    std::string op;
    std::optional<std::unique_ptr<Expression>> value;

    VariableAssignment(const std::string& identifier, const std::string& op, std::optional<std::unique_ptr<Expression>> value = std::nullopt)
        : identifier(identifier), op(op), value(std::move(value)) {}
};

struct BreakStatement {
    BreakStatement() {}
};

struct ContinueStatement {
    ContinueStatement() {}
};

struct ReturnStatement {
    std::optional<std::unique_ptr<Expression>> expression;

    ReturnStatement(std::optional<std::unique_ptr<Expression>> expression) : expression(std::move(expression)) {}
};

struct IfStatement;
struct WhileStatement;
struct ForStatement;

using Statement = std::variant<VariableDeclaration, VariableAssignment, IfStatement, WhileStatement, ForStatement, BreakStatement, ContinueStatement, ReturnStatement>;

struct IfStatement {
    std::unique_ptr<Expression> condition;
    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> thenBlock;
    std::vector<std::pair<std::unique_ptr<Expression>, std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>>>> elseifClauses;
    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> elseBlock;

    IfStatement(std::unique_ptr<Expression> condition,
        std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> thenBlock,
        std::vector<std::pair<std::unique_ptr<Expression>, std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>>>> elseifClauses = {},
        std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> elseBlock = {})
        : condition(std::move(condition)),
        thenBlock(std::move(thenBlock)),
        elseifClauses(std::move(elseifClauses)),
        elseBlock(std::move(elseBlock)) {}
};

struct WhileStatement {
    std::unique_ptr<Expression> condition;
    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> block;

    WhileStatement(std::unique_ptr<Expression> condition, std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> block)
        : condition(std::move(condition)), block(std::move(block)) {}
};

struct ForStatement {
    std::optional<std::unique_ptr<Statement>> initialization;
    std::optional<std::unique_ptr<Expression>> condition;
    std::optional<std::unique_ptr<Statement>> update;
    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> block;

    ForStatement(
        std::optional<std::unique_ptr<Statement>> initialization,
        std::optional<std::unique_ptr<Expression>> condition,
        std::optional<std::unique_ptr<Statement>> update,
        std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> block)
        : initialization(std::move(initialization)), condition(std::move(condition)), update(std::move(update)), block(std::move(block)) {}
};

struct Program {
    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> body;

    Program() : body{} {};
};

#endif // AST_HPP
