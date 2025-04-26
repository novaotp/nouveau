#include <iostream>
#include "ast.hpp"

StringType::StringType() {}
IntegerType::IntegerType() {}
FloatType::FloatType() {}
BooleanType::BooleanType() {}

std::string StringType::toString() const {
    return "string";
}

bool StringType::compare(const NodeType& other) const {
    return std::get_if<std::shared_ptr<StringType>>(&other) != nullptr;
}

std::string IntegerType::toString() const {
    return "int";
}

bool IntegerType::compare(const NodeType& other) const {
    return std::get_if<std::shared_ptr<IntegerType>>(&other) != nullptr;
}

std::string FloatType::toString() const {
    return "float";
}

bool FloatType::compare(const NodeType& other) const {
    return std::get_if<std::shared_ptr<FloatType>>(&other) != nullptr;
}

std::string BooleanType::toString() const {
    return "bool";
}

bool BooleanType::compare(const NodeType& other) const {
    return std::get_if<std::shared_ptr<BooleanType>>(&other) != nullptr;
}

NodePosition::NodePosition() : column(0), line(0) {};
NodePosition::NodePosition(size_t column, size_t line) : column(column), line(line) {};

NodeMetadata::NodeMetadata() : start(NodePosition()), end(NodePosition()) {};
NodeMetadata::NodeMetadata(NodePosition start, NodePosition end) : start(start), end(end) {};

StringLiteral::StringLiteral(NodeMetadata metadata, std::string value) : metadata(metadata), value(value) {};
IntLiteral::IntLiteral(NodeMetadata metadata, int value) : metadata(metadata), value(value) {};
FloatLiteral::FloatLiteral(NodeMetadata metadata, float value) : metadata(metadata), value(value) {};
BooleanLiteral::BooleanLiteral(NodeMetadata metadata, bool value) : metadata(metadata), value(value) {};
Identifier::Identifier(NodeMetadata metadata, std::string name) : metadata(metadata), name(name) {};
LogicalNotOperation::LogicalNotOperation(NodeMetadata metadata, std::shared_ptr<Expression> expression)
    : metadata(metadata), expression(std::move(expression)) {};
BinaryOperation::BinaryOperation(NodeMetadata metadata, std::shared_ptr<Expression> left, BinaryOperator op, std::shared_ptr<Expression> right)
    : metadata(metadata), lhs(std::move(left)), op(op), rhs(std::move(right)) {};
VariableDeclaration::VariableDeclaration(
    NodeMetadata metadata,
    bool isMutable,
    NodeType type,
    const std::string& identifier,
    std::optional<std::shared_ptr<Expression>> value = std::nullopt
) : metadata(metadata), isMutable(isMutable), type(type), identifier(identifier), value(std::move(value)) {};
VariableAssignment::VariableAssignment(
    NodeMetadata metadata,
    const std::string& identifier,
    const std::string& op,
    std::shared_ptr<Expression> value
) : metadata(metadata), identifier(identifier), op(op), value(std::move(value)) {};
Program::Program() : body{} {};

constexpr int SPACE_COUNT = 4;

void printExpression(const Expression& n, const size_t indentCount) {
    std::string indent(indentCount * SPACE_COUNT, ' ');

    std::visit([&indent, &indentCount](const auto& expression) {
        using ExpressionType = std::decay_t<decltype(expression)>;

        if constexpr (std::is_same_v<ExpressionType, StringLiteral>) {
            std::cout << indent << "String: " << expression.value << std::endl;
        } else if constexpr (std::is_same_v<ExpressionType, IntLiteral>) {
            std::cout << indent << "Int: " << expression.value << std::endl;
        } else if constexpr (std::is_same_v<ExpressionType, FloatLiteral>) {
            std::cout << indent << "Float: " << expression.value << std::endl;
        } else if constexpr (std::is_same_v<ExpressionType, BooleanLiteral>) {
            std::cout << indent << "Boolean: " << (expression.value ? "true" : "false") << std::endl;
        } else if constexpr (std::is_same_v<ExpressionType, Identifier>) {
            std::cout << indent << "Identifier: " << expression.name << std::endl;
        } else if constexpr (std::is_same_v<ExpressionType, BinaryOperation>) {
            std::cout << indent << "BinaryOperation" << std::endl;

            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 1);
            }, *expression.lhs);

            std::cout << indent + std::string(SPACE_COUNT, ' ') << "Operator: " << binaryOperatorToString(expression.op) << std::endl;

            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 1);
            }, *expression.rhs);
        } else if constexpr (std::is_same_v<ExpressionType, LogicalNotOperation>) {
            std::cout << indent << "NotOperation" << std::endl;

            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 1);
            }, *expression.expression);
        } else {
            std::cout << indent << "Unknown Expression Type" << std::endl;
        }
    }, n);
}

void printStatement(const Statement& n, const size_t indentCount) {
    std::string indent(indentCount * SPACE_COUNT, ' ');

    std::visit([&indent, &indentCount](const auto& statement) {
        using StatementType = std::decay_t<decltype(statement)>;

        if constexpr (std::is_same_v<StatementType, VariableDeclaration>) {
            std::cout << indent << "Variable Declaration" << std::endl;
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "IsMutable: " << (statement.isMutable ? "true" : "false") << std::endl;
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Type: ";

            std::cout << std::visit([](const auto& ptr) -> std::string {
                return ptr->toString();
            }, statement.type) << std::endl;

            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Identifier: " << statement.identifier << std::endl;

            if (statement.value.has_value()) {
                std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value" << std::endl;

                std::visit([&indentCount](const auto& expr) {
                    printExpression(expr, indentCount + 2);
                }, *statement.value.value());
            } else {
                std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value: undefined" << std::endl;
            }
        } else if constexpr (std::is_same_v<StatementType, VariableAssignment>) {

            std::cout << indent << "Variable Assignment" << std::endl;
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Identifier: " << statement.identifier << std::endl;
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Operator: " << statement.op << std::endl;
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value" << std::endl;

            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 2);
            }, *statement.value);
        } else {
            std::cout << indent << "Unknown Statement Type" << std::endl;
        }
    }, n);
}

void Program::prettyPrint() {
    for (const auto& node : this->body) {
        std::visit([](const auto& ptr) {
            using StatementOrExpression = std::decay_t<decltype(*ptr)>;

            if constexpr (std::is_same_v<StatementOrExpression, Expression>) {
                printExpression(*ptr, 0);
            } else if constexpr (std::is_same_v<StatementOrExpression, Statement>) {
                printStatement(*ptr, 0);
            } else {
                std::cout << "Unknown Node Type" << std::endl;
            }
        }, node);
    }
}

std::string binaryOperatorToString(BinaryOperator op) {
    if (op == ADDITION) return "+";
    if (op == SUBTRACTION) return "-";
    if (op == MULTIPLICATION) return "*";
    if (op == DIVISION) return "/";
    if (op == MODULO) return "%";
    if (op == EQUAL) return "==";
    if (op == NOT_EQUAL) return "!=";
    if (op == GREATER_THAN) return ">";
    if (op == GREATER_OR_EQUAL) return ">=";
    if (op == LESS_THAN) return "<";
    if (op == LESS_OR_EQUAL) return "<=";
    if (op == AND) return "&&";
    if (op == OR) return "||";

    throw std::runtime_error("Invalid operator to stringify");
};
