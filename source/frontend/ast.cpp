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
BinaryOperation::BinaryOperation(NodeMetadata metadata, std::shared_ptr<Expression> left, const std::string& op, std::shared_ptr<Expression> right)
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
    std::optional<std::shared_ptr<Expression>> value = std::nullopt
) : metadata(metadata), identifier(identifier), op(op), value(std::move(value)) {};
Program::Program() : body{} {};

constexpr int SPACE_COUNT = 4;

template <typename NodeType>
void printExpression(const NodeType& n, const size_t indentCount) {
    std::string indent(indentCount * SPACE_COUNT, ' ');

    if constexpr (std::is_same_v<NodeType, StringLiteral>) {
        std::cout << indent << "String: " << n.value << std::endl;
    } else if constexpr (std::is_same_v<NodeType, IntLiteral>) {
        std::cout << indent << "Int: " << n.value << std::endl;
    } else if constexpr (std::is_same_v<NodeType, FloatLiteral>) {
        std::cout << indent << "Float: " << n.value << std::endl;
    } else if constexpr (std::is_same_v<NodeType, BooleanLiteral>) {
        std::cout << indent << "Boolean: " << (n.value ? "true" : "false") << std::endl;
    } else if constexpr (std::is_same_v<NodeType, Identifier>) {
        std::cout << indent << "Identifier: " << n.name << std::endl;
    } else if constexpr (std::is_same_v<NodeType, BinaryOperation>) {
        std::cout << indent << "BinaryOperation" << std::endl;

        std::visit([&indentCount](const auto& expr) {
            printExpression(expr, indentCount + 1);
        }, *n.lhs);
        std::cout << indent + std::string(SPACE_COUNT, ' ') << "Operator: " << n.op << std::endl;
        std::visit([&indentCount](const auto& expr) {
            printExpression(expr, indentCount + 1);
        }, *n.rhs);
    } else if constexpr (std::is_same_v<NodeType, LogicalNotOperation>) {
        std::cout << indent << "NotOperation" << std::endl;
        std::visit([&indentCount](const auto& expr) {
            printExpression(expr, indentCount + 1);
        }, *n.expression);
    } else {
        std::cout << indent << "Unknown Expression Type" << std::endl;
    }
}

template <typename NodeType>
void printStatement(const NodeType& n, const size_t indentCount) {
    std::string indent(indentCount * SPACE_COUNT, ' ');

    if constexpr (std::is_same_v<NodeType, VariableDeclaration>) {
        std::cout << indent << "Variable Declaration" << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "IsMutable: " << (n.isMutable ? "true" : "false") << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Type: ";

        std::cout << std::visit([](const auto& ptr) -> std::string {
            return ptr->toString();
        }, n.type) << std::endl;

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Identifier: " << n.identifier << std::endl;

        if (n.value.has_value()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value" << std::endl;

            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 2);
            }, *n.value.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value: null" << std::endl;
        }
    } else if constexpr (std::is_same_v<NodeType, VariableAssignment>) {

        std::cout << indent << "Variable Assignment" << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Identifier: " << n.identifier << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Operator: " << n.op << std::endl;

        if (n.value.has_value()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value" << std::endl;

            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 2);
            }, *n.value.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value: null" << std::endl;
        }
    } else {
        std::cout << indent << "Unknown Statement Type" << std::endl;
    }
}

void Program::prettyPrint() {
    for (size_t i = 0; i < this->body.size(); i++) {
        const auto& node = this->body[i];

        std::visit([](const auto& ptr) {
            using Type = std::decay_t<decltype(*ptr)>;

            if constexpr (std::is_same_v<Type, Expression>) {
                std::visit([](const auto& expr) {
                    printExpression(expr, 0);
                }, *ptr);
            } else if constexpr (std::is_same_v<Type, Statement>) {
                std::visit([](const auto& expr) {
                    printStatement(expr, 0);
                }, *ptr);
            } else {
                std::cout << "Unknown Node Type" << std::endl;
            }
        }, node);
    }
}
