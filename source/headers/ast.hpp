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
struct VectorType;
struct FunctionType;
struct OptionalType;
struct UnionType;
struct VoidType;

// ? Might want to find a better name
using NodeType = std::variant<
    std::shared_ptr<StringType>,
    std::shared_ptr<IntegerType>,
    std::shared_ptr<FloatType>,
    std::shared_ptr<BooleanType>,
    std::shared_ptr<VectorType>,
    std::shared_ptr<FunctionType>,
    std::shared_ptr<OptionalType>,
    std::shared_ptr<UnionType>,
    std::shared_ptr<VoidType>
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

struct VoidType : public Type {
    VoidType();

    std::string toString() const override;

    /// @brief Compares the type between `this` and `other`.
    /// @param other The type to compare with.
    /// @return `true` if the types are equal, `false` otherwise.
    bool compare(const NodeType& other) const override;
};

struct VectorType : public Type {
    NodeType valueType;

    VectorType(NodeType valueType);

    std::string toString() const override;

    /// @brief Deeply compares the type between `this` and `other`.
    /// @param other The type to compare with.
    /// @return `true` if the types are equal, `false` otherwise.
    /// @note This implementation also compares the underlying type of the vectors.
    bool compare(const NodeType& other) const override;
};

struct FunctionType : public Type {
    std::vector<NodeType> parameterTypes;
    NodeType returnType;

    /// @brief A shorthand constructor for creating a function type with no parameters.
    FunctionType(NodeType returnType);
    FunctionType(std::vector<NodeType> parameterTypes, NodeType returnType);

    std::string toString() const override;

    /// @brief Deeply compares the type between `this` and `other`.
    /// @param other The type to compare with.
    /// @return `true` if the types are equal, `false` otherwise.
    /// @note This implementation also compares the return type and parameter types of the functions.
    bool compare(const NodeType& other) const override;
};

struct OptionalType : public Type {
    NodeType containedType;

    OptionalType(NodeType containedType);

    std::string toString() const override;

    /// @brief Deeply compares the type between `this` and `other`.
    /// @param other The type to compare with.
    /// @return `true` if the types are equal, `false` otherwise.
    /// @note This implementation also compares the underlying type of the optionals.
    bool compare(const NodeType& other) const override;
};

struct UnionType : public Type {
    std::vector<NodeType> containedTypes;

    UnionType(std::vector<NodeType> containedTypes);

    std::string toString() const override;

    /// @brief Deeply compares the type between `this` and `other`.
    /// @param other The type to compare with.
    /// @return `true` if the types are equal, `false` otherwise.
    /// @note This implementation also compares the underlying types of the unions.
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

struct NullLiteral {
    NodeMetadata metadata;

    NullLiteral(NodeMetadata metadata);
};

struct Identifier {
    NodeMetadata metadata;
    std::string name;

    Identifier(NodeMetadata metadata, std::string name);
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

    Vector(NodeMetadata metadata, std::vector<std::shared_ptr<Expression>> values);
};

struct LogicalNotOperation {
    NodeMetadata metadata;
    std::shared_ptr<Expression> expression;

    LogicalNotOperation(NodeMetadata metadata, std::shared_ptr<Expression> expression);
};

struct BinaryOperation {
    NodeMetadata metadata;
    std::shared_ptr<Expression> lhs;
    std::string op;
    std::shared_ptr<Expression> rhs;

    BinaryOperation(NodeMetadata metadata, std::shared_ptr<Expression> left, const std::string& op, std::shared_ptr<Expression> right);
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

struct BreakStatement {
    NodeMetadata metadata;

    BreakStatement(NodeMetadata metadata);
};

struct ContinueStatement {
    NodeMetadata metadata;

    ContinueStatement(NodeMetadata metadata);
};

struct ReturnStatement {
    NodeMetadata metadata;
    std::optional<std::shared_ptr<Expression>> expression;

    ReturnStatement(NodeMetadata metadata);
    ReturnStatement(NodeMetadata metadata, std::optional<std::shared_ptr<Expression>> expression);
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
        std::vector<std::pair<std::shared_ptr<Expression>, std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>>>> elseifClauses,
        std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> elseBlock
    );
};

struct WhileStatement {
    NodeMetadata metadata;
    std::shared_ptr<Expression> condition;
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> block;

    WhileStatement(
        NodeMetadata metadata,
        std::shared_ptr<Expression> condition,
        std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> block
    );
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
    );
};

/// @attention This is an expression, such that you can assign functions to variables.
struct Function {
    NodeMetadata metadata;
    NodeType returnType;
    /// Is `null` if the function is an anonymous function.
    std::optional<std::string> name;
    std::vector<std::shared_ptr<VariableDeclaration>> parameters;
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> body;

    Function(
        NodeMetadata metadata, NodeType returnType,
        std::optional<std::string> name,
        std::vector<std::shared_ptr<VariableDeclaration>> parameters,
        std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> body
    );
};

struct FunctionCall {
    NodeMetadata metadata;
    std::string identifier;
    std::vector<std::shared_ptr<Expression>> arguments;

    FunctionCall(NodeMetadata metadata, const std::string& identifier, std::vector<std::shared_ptr<Expression>> arguments);
};

struct Program {
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> body;

    Program();

    void prettyPrint();
};

#endif // AST_HPP
