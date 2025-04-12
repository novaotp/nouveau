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

struct Node {
    virtual ~Node() = default;
    virtual std::string codegen() const = 0;
};

struct StringLiteral : public Node {
    NodeMetadata metadata;
    std::string value;

    StringLiteral(NodeMetadata metadata, std::string value);

    std::string codegen() const;
};

struct IntLiteral : public Node {
    NodeMetadata metadata;
    int value;

    IntLiteral(NodeMetadata metadata, int value);

    std::string codegen() const;
};

struct FloatLiteral : public Node {
    NodeMetadata metadata;
    float value;

    FloatLiteral(NodeMetadata metadata, float value);

    std::string codegen() const;
};

struct BooleanLiteral : public Node {
    NodeMetadata metadata;
    bool value;

    BooleanLiteral(NodeMetadata metadata, bool value);

    std::string codegen() const;
};

struct Identifier : public Node {
    NodeMetadata metadata;
    std::string name;

    Identifier(NodeMetadata metadata, std::string name);

    std::string codegen() const;
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

struct LogicalNotOperation : public Node {
    NodeMetadata metadata;
    std::shared_ptr<Expression> expression;

    LogicalNotOperation(NodeMetadata metadata, std::shared_ptr<Expression> expression);

    std::string codegen() const;
};

struct BinaryOperation : public Node {
    NodeMetadata metadata;
    std::shared_ptr<Expression> lhs;
    std::string op;
    std::shared_ptr<Expression> rhs;

    BinaryOperation(NodeMetadata metadata, std::shared_ptr<Expression> left, const std::string& op, std::shared_ptr<Expression> right);

    std::string codegen() const;
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
    std::optional<std::shared_ptr<Expression>> value;

    VariableAssignment(
        NodeMetadata metadata,
        const std::string& identifier,
        const std::string& op,
        std::optional<std::shared_ptr<Expression>> value
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
