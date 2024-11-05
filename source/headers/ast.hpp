#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <optional>

struct NodePosition {
    size_t column;
    size_t line;

    NodePosition() : column(0), line(0) {}
    NodePosition(size_t column, size_t line) : column(column), line(line) {}
};

struct NodeMetadata {
    NodePosition start;
    NodePosition end;

    NodeMetadata() : start(NodePosition()), end(NodePosition()) {}
    NodeMetadata(NodePosition start, NodePosition end) : start(start), end(end) {}
};

struct StringLiteral {
    NodeMetadata metadata;
    std::string value;

    StringLiteral(NodeMetadata metadata, std::string value) : metadata(metadata), value(value) {};
};

struct IntLiteral {
    NodeMetadata metadata;
    int value;

    IntLiteral(NodeMetadata metadata, int value) : metadata(metadata), value(value) {};
};

struct FloatLiteral {
    NodeMetadata metadata;
    float value;

    FloatLiteral(NodeMetadata metadata, float value) : metadata(metadata), value(value) {};
};

struct BooleanLiteral {
    NodeMetadata metadata;
    bool value;

    BooleanLiteral(NodeMetadata metadata, bool value) : metadata(metadata), value(value) {};
};

struct NullLiteral {
    NodeMetadata metadata;
    NullLiteral(NodeMetadata metadata) : metadata(metadata) {};
};

struct Identifier {
    NodeMetadata metadata;
    std::string name;

    Identifier(NodeMetadata metadata, std::string name) : metadata(metadata), name(name) {}
};

struct LogicalNotOperation;
struct BinaryOperation;
struct Vector;
struct Function;
struct FunctionCall;

using Expression = std::variant<
    Function,
    FunctionCall,
    BinaryOperation,
    LogicalNotOperation,
    Vector,
    Identifier,
    StringLiteral,
    IntLiteral,
    FloatLiteral,
    BooleanLiteral,
    NullLiteral
>;

struct Vector {
    NodeMetadata metadata;
    std::vector<std::shared_ptr<Expression>> values;

    Vector(NodeMetadata metadata, std::vector<std::shared_ptr<Expression>> values)
        : metadata(metadata), values(std::move(values)) {}
};

struct LogicalNotOperation {
    NodeMetadata metadata;
    std::shared_ptr<Expression> expression;

    LogicalNotOperation(NodeMetadata metadata, std::shared_ptr<Expression> expression)
        : metadata(metadata), expression(std::move(expression)) {};
};

struct BinaryOperation {
    NodeMetadata metadata;
    std::shared_ptr<Expression> lhs;
    std::string op;
    std::shared_ptr<Expression> rhs;

    BinaryOperation(NodeMetadata metadata, std::shared_ptr<Expression> left, const std::string& op, std::shared_ptr<Expression> right)
        : metadata(metadata), lhs(std::move(left)), op(op), rhs(std::move(right)) {}
};

struct VariableDeclaration {
    NodeMetadata metadata;
    bool isMutable;
    std::string type;
    std::string identifier;
    std::optional<std::shared_ptr<Expression>> value;

    VariableDeclaration(
        NodeMetadata metadata,
        bool isMutable,
        const std::string& type,
        const std::string& identifier,
        std::optional<std::shared_ptr<Expression>> value = std::nullopt
    ) : metadata(metadata), isMutable(isMutable), type(type), identifier(identifier), value(std::move(value)) {}
};

struct VariableAssignment {
    NodeMetadata metadata;
    std::string identifier;
    std::string op;
    std::optional<std::shared_ptr<Expression>> value;

    VariableAssignment(
        NodeMetadata metadata,
        const std::string& identifier,
        const std::string& op,
        std::optional<std::shared_ptr<Expression>> value = std::nullopt
    ) : metadata(metadata), identifier(identifier), op(op), value(std::move(value)) {}
};

struct BreakStatement {
    NodeMetadata metadata;
    BreakStatement(NodeMetadata metadata) : metadata(metadata) {}
};

struct ContinueStatement {
    NodeMetadata metadata;
    ContinueStatement(NodeMetadata metadata) : metadata(metadata) {}
};

struct ReturnStatement {
    NodeMetadata metadata;
    std::optional<std::shared_ptr<Expression>> expression;

    ReturnStatement(NodeMetadata metadata, std::optional<std::shared_ptr<Expression>> expression)
        : metadata(metadata), expression(std::move(expression)) {}
};

struct IfStatement;
struct WhileStatement;
struct ForStatement;

using Statement = std::variant<
    VariableDeclaration,
    VariableAssignment,
    IfStatement,
    WhileStatement,
    ForStatement,
    BreakStatement,
    ContinueStatement,
    ReturnStatement
>;

struct IfStatement {
    NodeMetadata metadata;
    std::shared_ptr<Expression> condition;
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> thenBlock;
    std::vector<std::pair<std::shared_ptr<Expression>, std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>>>> elseifClauses;
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> elseBlock;

    IfStatement(
        NodeMetadata metadata, std::shared_ptr<Expression> condition,
        std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> thenBlock,
        std::vector<std::pair<std::shared_ptr<Expression>, std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>>>> elseifClauses = {},
        std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> elseBlock = {}
    ) : metadata(metadata),
        condition(std::move(condition)),
        thenBlock(std::move(thenBlock)),
        elseifClauses(std::move(elseifClauses)),
        elseBlock(std::move(elseBlock)) {}
};

struct WhileStatement {
    NodeMetadata metadata;
    std::shared_ptr<Expression> condition;
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> block;

    WhileStatement(
        NodeMetadata metadata,
        std::shared_ptr<Expression> condition,
        std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> block
    ) : metadata(metadata), condition(std::move(condition)), block(std::move(block)) {}
};

struct ForStatement {
    NodeMetadata metadata;
    std::optional<std::shared_ptr<Statement>> initialization;
    std::optional<std::shared_ptr<Expression>> condition;
    std::optional<std::shared_ptr<Statement>> update;
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> block;

    ForStatement(
        NodeMetadata metadata,
        std::optional<std::shared_ptr<Statement>> initialization,
        std::optional<std::shared_ptr<Expression>> condition,
        std::optional<std::shared_ptr<Statement>> update,
        std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> block
    ) : metadata(metadata),
        initialization(std::move(initialization)),
        condition(std::move(condition)),
        update(std::move(update)),
        block(std::move(block)) {}
};

/// @attention This is an expression, such that you can assign functions to variables.
struct Function {
    NodeMetadata metadata;
    std::string returnType;
    std::string name;
    std::vector<std::shared_ptr<VariableDeclaration>> parameters;
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> body;

    Function(
        NodeMetadata metadata, const std::string& returnType,
        const std::string& name,
        std::vector<std::shared_ptr<VariableDeclaration>> parameters,
        std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> body
    ) : metadata(metadata), returnType(returnType), name(name), parameters(std::move(parameters)), body(std::move(body)) {}
};

struct FunctionCall {
    NodeMetadata metadata;
    std::string identifier;
    std::vector<std::shared_ptr<Expression>> arguments;

    FunctionCall(NodeMetadata metadata, const std::string& identifier, std::vector<std::shared_ptr<Expression>> arguments)
        : metadata(metadata), identifier(identifier), arguments(std::move(arguments)) {}
};

struct Program {
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> body;

    Program() : body{} {};

    void prettyPrint();
};

#endif // AST_HPP
