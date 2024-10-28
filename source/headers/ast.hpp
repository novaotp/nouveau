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

struct LogicalNotOperation;
struct BinaryOperation;

using Expression = std::variant<BinaryOperation, LogicalNotOperation, StringLiteral, IntLiteral, FloatLiteral, BooleanLiteral, NullLiteral>;

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
    std::optional<std::unique_ptr<Expression>> value;

    VariableAssignment(const std::string& identifier, std::optional<std::unique_ptr<Expression>> value = std::nullopt)
        : identifier(identifier), value(std::move(value)) {}
};

using Statement = std::variant<VariableDeclaration, VariableAssignment>;

struct Program {
    std::vector<std::variant<std::unique_ptr<Expression>, std::unique_ptr<Statement>>> body;

    Program() : body{} {};
};
