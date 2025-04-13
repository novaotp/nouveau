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

StringLiteral::StringLiteral(NodeMetadata metadata, std::string value) : metadata(metadata), value(value) {}
Value StringLiteral::codegen() const {
    return Value({}, {}, {});
};

IntLiteral::IntLiteral(NodeMetadata metadata, int value) : metadata(metadata), value(value) {}
Value IntLiteral::codegen() const {
    return Value({}, {}, { std::string("mov rax, ") + std::to_string(this->value) });
};

FloatLiteral::FloatLiteral(NodeMetadata metadata, float value) : metadata(metadata), value(value) {}
Value FloatLiteral::codegen() const {
    return Value({}, {}, { std::string("movsd xmm0, ") + std::to_string(this->value) });
};

BooleanLiteral::BooleanLiteral(NodeMetadata metadata, bool value) : metadata(metadata), value(value) {}
Value BooleanLiteral::codegen() const {
    return Value({}, {}, {});
};

Identifier::Identifier(NodeMetadata metadata, std::string name) : metadata(metadata), name(name) {}
Value Identifier::codegen() const {
    return Value({}, {}, {});
};

LogicalNotOperation::LogicalNotOperation(NodeMetadata metadata, std::shared_ptr<Expression> expression)
    : metadata(metadata), expression(std::move(expression)) {}
Value LogicalNotOperation::codegen() const {
    return Value({}, {}, {});
};

BinaryOperation::BinaryOperation(NodeMetadata metadata, std::shared_ptr<Expression> left, const std::string& op, std::shared_ptr<Expression> right)
    : metadata(metadata), lhs(std::move(left)), op(op), rhs(std::move(right)) {}
Value BinaryOperation::codegen() const {
    return Value({}, {}, {});
};

VariableDeclaration::VariableDeclaration(
    NodeMetadata metadata,
    bool isMutable,
    NodeType type,
    const std::string& identifier,
    std::optional<std::shared_ptr<Expression>> value = std::nullopt
) : metadata(metadata), isMutable(isMutable), type(type), identifier(identifier), value(std::move(value)) {};
Value VariableDeclaration::codegen() const {
    if (!this->isMutable && this->value.has_value()) {
        std::string data = std::visit([&](const auto& dataType) -> std::string {
            using DataType = std::decay_t<decltype(*dataType)>;

            std::string operand;
            if constexpr (std::is_same_v<DataType, IntegerType> || std::is_same_v<DataType, FloatType>) {
                operand = "dq";
            } else if constexpr (std::is_same_v<DataType, StringType>) {
                operand = "db";
            }

            return this->identifier + std::string(" ") + operand + std::string(" ") + std::visit([&](const auto& value) -> std::string {
                using ValueType = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<ValueType, IntLiteral> || std::is_same_v<ValueType, FloatLiteral>) {
                    return std::to_string(value.value);
                } else if constexpr (std::is_same_v<ValueType, StringLiteral>) {
                    return std::string("\"") + value.value + std::string("\", 0xA")
                    + std::string("\n\t") + this->identifier + std::string("_length equ $ - ") + this->identifier;
                } else {
                    return std::string("");
                }
            }, *(this->value.value()));
        }, this->type);

        /* std::string instructions = std::visit([&](const auto& value) -> std::string {
            using ValueType = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<ValueType, IntLiteral>) {
                return std::string("mov rax, [") + this->identifier + std::string("]");
            } else if constexpr (std::is_same_v<ValueType, FloatLiteral>) {
                return std::string("movsd xmm0, [") + this->identifier + std::string("]");
            } else {
                return std::string("");
            }
        }, *(this->value.value())); */

        return Value({ data }, {}, { });
    } else {
        return Value({}, {}, {});
    }
};

std::string generateTempName() {
    static int counter = 0;
    return "_tmp" + std::to_string(counter++);
}

VariableAssignment::VariableAssignment(
    NodeMetadata metadata,
    const std::string& identifier,
    const std::string& op,
    std::shared_ptr<Expression> value
) : metadata(metadata), identifier(identifier), op(op), value(std::move(value)) {};
Value VariableAssignment::codegen() const {
    std::vector<std::string> data = {};
    std::vector<std::string> instructions = {};

    std::visit([&](const auto& value) {
        using ValueType = std::decay_t<decltype(value)>;

        std::cout << typeid(ValueType).name() << std::endl;

        if constexpr (std::is_same_v<ValueType, IntLiteral>) {
            instructions.push_back(std::string("mov qword [") + this->identifier + std::string("], ") + std::to_string(value.value));
        }/*  else if constexpr (std::is_same_v<ValueType, FloatLiteral>) {
            auto new_identifier = generateTempName();

            std::optional<std::shared_ptr<Expression>> new_value = this->value;
            VariableDeclaration(this->metadata, false, FloatType(), this->identifier, new_value);

            instructions.push_back(std::string("movsd [") + this->identifier + std::string("], ") + std::to_string(value.value));
        } */ else {

        }
    }, *(this->value));

    return Value({}, {}, instructions);
};

Program::Program() : body{} {};

Value::Value(std::vector<std::string> data, std::vector<std::string> bss, std::vector<std::string> instructions)
    : data(data), bss(bss), instructions(instructions) {};

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

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Value" << std::endl;

        std::visit([&indentCount](const auto& expr) {
            printExpression(expr, indentCount + 2);
        }, *(n.value));
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
