#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <optional>

struct StringType;
struct IntegerType;
struct FloatType;
struct BooleanType;

// ? Might want to find a better name
using NodeType = std::variant<
                 std::shared_ptr<StringType>,
                 std::shared_ptr<IntegerType>,
                 std::shared_ptr<FloatType>,
                 std::shared_ptr<BooleanType>
                 >;

/** The base type for a type. */
struct Type {
    virtual ~Type() = default;
    virtual std::string toString() const = 0;
    virtual bool compare(const NodeType& other) const = 0;
};

struct StringType : public Type {
    StringType();

    std::string toString() const override;

    /// @brief Compares the type between `this` and `other`.
    /// @param other The type to compare with.
    /// @return `true` if the types are equal, `false` otherwise.
    bool compare(const NodeType& other) const override;
};

struct IntegerType : public Type {
    IntegerType();

    std::string toString() const override;

    /// @brief Compares the type between `this` and `other`.
    /// @param other The type to compare with.
    /// @return `true` if the types are equal, `false` otherwise.
    bool compare(const NodeType& other) const override;
};

struct FloatType : public Type {
    FloatType();

    std::string toString() const override;

    /// @brief Compares the type between `this` and `other`.
    /// @param other The type to compare with.
    /// @return `true` if the types are equal, `false` otherwise.
    bool compare(const NodeType& other) const override;
};

struct BooleanType : public Type {
    BooleanType();

    std::string toString() const override;

    /// @brief Compares the type between `this` and `other`.
    /// @param other The type to compare with.
    /// @return `true` if the types are equal, `false` otherwise.
    bool compare(const NodeType& other) const override;
};

struct NodePosition {
    size_t column;
    size_t line;

    NodePosition();
    NodePosition(size_t column, size_t line);
};

struct NodeMetadata {
    NodePosition start;
    NodePosition end;

    NodeMetadata();
    NodeMetadata(NodePosition start, NodePosition end);
};

struct StringLiteral {
    NodeMetadata metadata;
    std::string value;

    StringLiteral(NodeMetadata metadata, std::string value);
};

struct IntLiteral {
    NodeMetadata metadata;
    int value;

    IntLiteral(NodeMetadata metadata, int value);
};

struct FloatLiteral {
    NodeMetadata metadata;
    float value;

    FloatLiteral(NodeMetadata metadata, float value);
};

struct BooleanLiteral {
    NodeMetadata metadata;
    bool value;

    BooleanLiteral(NodeMetadata metadata, bool value);
};

struct Identifier {
    NodeMetadata metadata;
    std::string name;

    Identifier(NodeMetadata metadata, std::string name);
};

struct LogicalNotOperation;
struct BinaryOperation;

using Expression = std::variant<
                   BinaryOperation,
                   LogicalNotOperation,
                   Identifier,
                   StringLiteral,
                   IntLiteral,
                   FloatLiteral,
                   BooleanLiteral
                   >;

struct LogicalNotOperation {
    NodeMetadata metadata;
    std::shared_ptr<Expression> expression;

    LogicalNotOperation(NodeMetadata metadata, std::shared_ptr<Expression> expression);
};

enum BinaryOperator {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    MODULO,
    EQUAL,
    NOT_EQUAL,
    GREATER_THAN,
    GREATER_OR_EQUAL,
    LESS_THAN,
    LESS_OR_EQUAL,
    AND,
    OR
};

std::string binaryOperatorToString(BinaryOperator op);

struct BinaryOperation {
    NodeMetadata metadata;
    std::shared_ptr<Expression> lhs;
    BinaryOperator op;
    std::shared_ptr<Expression> rhs;

    BinaryOperation(NodeMetadata metadata, std::shared_ptr<Expression> left, BinaryOperator op, std::shared_ptr<Expression> right);
};

struct VariableDeclaration {
    NodeMetadata metadata;
    bool isMutable;
    NodeType type;
    std::string identifier;
    std::optional<std::shared_ptr<Expression>> value;

    VariableDeclaration(
        NodeMetadata metadata,
        bool isMutable,
        NodeType type,
        const std::string& identifier,
        std::optional<std::shared_ptr<Expression>> value
    );
};

struct VariableAssignment {
    NodeMetadata metadata;
    std::string identifier;
    std::string op;
    std::shared_ptr<Expression> value;

    VariableAssignment(
        NodeMetadata metadata,
        const std::string& identifier,
        const std::string& op,
        std::shared_ptr<Expression> value
    );
};

using Statement = std::variant<
                  VariableDeclaration,
                  VariableAssignment
                  >;

struct Program {
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> body;

    Program();

    void prettyPrint();
};

#endif // AST_HPP
