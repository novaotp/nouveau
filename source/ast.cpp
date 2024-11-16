#include <iostream>
#include "ast.hpp"

StringType::StringType() {}
IntegerType::IntegerType() {}
FloatType::FloatType() {}
BooleanType::BooleanType() {}
VoidType::VoidType() {}
VectorType::VectorType(NodeType valueType) : valueType(std::move(valueType)) {}
FunctionType::FunctionType(NodeType returnType) : parameterTypes{}, returnType(std::move(returnType)) {}
FunctionType::FunctionType(std::vector<NodeType> parameterTypes, NodeType returnType)
    : parameterTypes(std::move(parameterTypes)), returnType(std::move(returnType)) {}
OptionalType::OptionalType(NodeType containedType) : containedType(std::move(containedType)) {}
UnionType::UnionType(std::vector<NodeType> containedTypes) : containedTypes(std::move(containedTypes)) {}

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

std::string VoidType::toString() const {
    return "void";
}

bool VoidType::compare(const NodeType& other) const {
    return std::get_if<std::shared_ptr<VoidType>>(&other) != nullptr;
}

std::string VectorType::toString() const {
    return std::visit([](const auto& typePtr) { return typePtr->toString(); }, valueType) + "[]";
}

bool VectorType::compare(const NodeType& other) const {
    if (const auto* otherVector = std::get_if<std::shared_ptr<VectorType>>(&other)) {
        return std::visit([](const auto& thisValuePtr, const auto& otherValuePtr) -> bool {
            return thisValuePtr->compare(otherValuePtr);
        }, this->valueType, (*otherVector)->valueType);
    }

    return false;
}

std::string FunctionType::toString() const {
    std::string result = std::visit([](const auto& typePtr) { return typePtr->toString(); }, returnType) + " (";

    for (size_t i = 0; i < parameterTypes.size(); i++) {
        result += std::visit([](const auto& typePtr) { return typePtr->toString(); }, parameterTypes[i]);
        if (i < parameterTypes.size() - 1) {
            result += ", ";
        }
    }

    result += ")";

    return result;
}

bool FunctionType::compare(const NodeType& other) const {
    if (const auto* otherFunction = std::get_if<std::shared_ptr<FunctionType>>(&other)) {
        if (parameterTypes.size() != (*otherFunction)->parameterTypes.size()) {
            return false;
        }

        bool isReturnTypeMatching = std::visit([](const auto& thisReturnTypePtr, const auto& otherReturnTypePtr) -> bool {
            return thisReturnTypePtr->compare(otherReturnTypePtr);
        }, returnType, (*otherFunction)->returnType);

        if (!isReturnTypeMatching) {
            return false;
        }

        bool isMatching = true;

        for (size_t i = 0; i < parameterTypes.size(); i++) {
            isMatching &= std::visit([](const auto& thisParameterTypePtr, const auto& otherParameterTypePtr) -> bool {
                return thisParameterTypePtr->compare(otherParameterTypePtr);
            }, parameterTypes[i], (*otherFunction)->parameterTypes[i]);
        }

        return isMatching;
    }

    return false;
}

std::string OptionalType::toString() const {
    std::string result = "";

    result += std::visit([](const auto& typePtr) -> std::string {
        using TypePtr = std::decay_t<decltype(typePtr)>;

        if constexpr (std::is_same_v<TypePtr, StringType> ||
            std::is_same_v<TypePtr, IntegerType> ||
            std::is_same_v<TypePtr, FloatType> ||
            std::is_same_v<TypePtr, BooleanType> ||
            std::is_same_v<TypePtr, VoidType>
            ) {
            return typePtr->toString();
        }

        return "(" + typePtr->toString() + ")";
    }, containedType);

    return result + "?";
}

bool OptionalType::compare(const NodeType& other) const {
    if (const auto* otherOptional = std::get_if<std::shared_ptr<OptionalType>>(&other)) {
        return std::visit([](const auto& thisTypePtr, const auto& otherTypePtr) -> bool {
            return thisTypePtr->compare(otherTypePtr);
        }, this->containedType, (*otherOptional)->containedType);
    }

    return false;
}

std::string UnionType::toString() const {
    std::string result = "";

    for (size_t i = 0; i < containedTypes.size(); ++i) {
        result += std::visit([](const auto& typePtr) -> std::string {
            return typePtr->toString();
        }, containedTypes[i]);

        if (i < containedTypes.size() - 1) {
            result += "|";
        }
    }

    return result;
}

bool UnionType::compare(const NodeType& other) const {
    if (const auto* otherUnion = std::get_if<std::shared_ptr<UnionType>>(&other)) {
        if (containedTypes.size() != (*otherUnion)->containedTypes.size()) {
            return false;
        }

        bool isMatching = true;

        for (size_t i = 0; i < containedTypes.size(); i++) {
            isMatching &= std::visit([](const auto& thisTypePtr, const auto& otherTypePtr) -> bool {
                return thisTypePtr->compare(otherTypePtr);
            }, containedTypes[i], (*otherUnion)->containedTypes[i]);
        }

        return isMatching;
    }

    return false;
}

NodePosition::NodePosition() : column(0), line(0) {};
NodePosition::NodePosition(size_t column, size_t line) : column(column), line(line) {};

NodeMetadata::NodeMetadata() : start(NodePosition()), end(NodePosition()) {};
NodeMetadata::NodeMetadata(NodePosition start, NodePosition end) : start(start), end(end) {};

StringLiteral::StringLiteral(NodeMetadata metadata, std::string value) : metadata(metadata), value(value) {};
IntLiteral::IntLiteral(NodeMetadata metadata, int value) : metadata(metadata), value(value) {};
FloatLiteral::FloatLiteral(NodeMetadata metadata, float value) : metadata(metadata), value(value) {};
BooleanLiteral::BooleanLiteral(NodeMetadata metadata, bool value) : metadata(metadata), value(value) {};
NullLiteral::NullLiteral(NodeMetadata metadata) : metadata(metadata) {};
Identifier::Identifier(NodeMetadata metadata, std::string name) : metadata(metadata), name(name) {};
Vector::Vector(NodeMetadata metadata, std::vector<std::shared_ptr<Expression>> values)
    : metadata(metadata), values(std::move(values)) {}
LogicalNotOperation::LogicalNotOperation(NodeMetadata metadata, std::shared_ptr<Expression> expression)
    : metadata(metadata), expression(std::move(expression)) {};
BinaryOperation::BinaryOperation(NodeMetadata metadata, std::shared_ptr<Expression> left, const std::string& op, std::shared_ptr<Expression> right)
    : metadata(metadata), lhs(std::move(left)), op(op), rhs(std::move(right)) {}
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
) : metadata(metadata), identifier(identifier), op(op), value(std::move(value)) {}
BreakStatement::BreakStatement(NodeMetadata metadata) : metadata(metadata) {};
ContinueStatement::ContinueStatement(NodeMetadata metadata) : metadata(metadata) {};
ReturnStatement::ReturnStatement(NodeMetadata metadata) : metadata(metadata), expression(std::nullopt) {};
ReturnStatement::ReturnStatement(NodeMetadata metadata, std::optional<std::shared_ptr<Expression>> expression)
    : metadata(metadata), expression(std::move(expression)) {}
IfStatement::IfStatement(
    NodeMetadata metadata, std::shared_ptr<Expression> condition,
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> thenBlock,
    std::vector<std::pair<std::shared_ptr<Expression>, std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>>>> elseifClauses = {},
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> elseBlock = {}
) : metadata(metadata),
condition(std::move(condition)),
thenBlock(std::move(thenBlock)),
elseifClauses(std::move(elseifClauses)),
elseBlock(std::move(elseBlock)) {};
WhileStatement::WhileStatement(
    NodeMetadata metadata,
    std::shared_ptr<Expression> condition,
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> block
) : metadata(metadata), condition(std::move(condition)), block(std::move(block)) {};
ForStatement::ForStatement(
    NodeMetadata metadata,
    std::optional<std::shared_ptr<Statement>> initialization,
    std::optional<std::shared_ptr<Expression>> condition,
    std::optional<std::shared_ptr<Statement>> update,
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> block
) : metadata(metadata),
initialization(std::move(initialization)),
condition(std::move(condition)),
update(std::move(update)),
block(std::move(block)) {};
Function::Function(
    NodeMetadata metadata, NodeType returnType,
    std::optional<std::string> name,
    std::vector<std::shared_ptr<VariableDeclaration>> parameters,
    std::vector<std::variant<std::shared_ptr<Expression>, std::shared_ptr<Statement>>> body
) : metadata(metadata), returnType(returnType), name(name), parameters(std::move(parameters)), body(std::move(body)) {};
FunctionCall::FunctionCall(NodeMetadata metadata, const std::string& identifier, std::vector<std::shared_ptr<Expression>> arguments)
    : metadata(metadata), identifier(identifier), arguments(std::move(arguments)) {};
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
    } else if constexpr (std::is_same_v<NodeType, NullLiteral>) {
        std::cout << indent << "Null" << std::endl;
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
    } else if constexpr (std::is_same_v<NodeType, Vector>) {
        std::cout << indent << "Vector" << std::endl;

        for (const auto& expr : n.values) {
            std::visit([&indentCount](const auto& element) {
                printExpression(element, indentCount + 1);
            }, *expr);
        }
    } else if constexpr (std::is_same_v<NodeType, Function>) {
        std::cout << indent << "Function" << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Name: " << (n.name.has_value() ? n.name.value() : "Anonymous Function") << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Return Type: ";

        std::cout << std::visit([](const auto& ptr) -> std::string {
            return ptr->toString();
        }, n.returnType) << std::endl;

        if (n.parameters.empty()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Parameters: None" << std::endl;
        } else {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Parameters" << std::endl;

            for (const auto& param : n.parameters) {
                printStatement(*param, indentCount + 2);
            }
        }

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Body" << std::endl;
        for (const auto& element : n.body) {
            std::visit([&indentCount](const auto& item) {
                using ItemType = std::decay_t<decltype(*item)>;

                if constexpr (std::is_same_v<ItemType, Statement>) {
                    std::visit([&indentCount](const auto& expr) {
                        printStatement(expr, indentCount + 2);
                    }, *item);
                } else if constexpr (std::is_same_v<ItemType, Expression>) {
                    std::visit([&indentCount](const auto& expr) {
                        printExpression(expr, indentCount + 2);
                    }, *item);
                } else {
                    std::cout << "Unknown Type in Body" << std::endl;
                }
            }, element);
        }
    } else if constexpr (std::is_same_v<NodeType, FunctionCall>) {
        std::cout << indent << "Function Call" << std::endl;
        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Identifier: " << n.identifier << std::endl;

        if (n.arguments.empty()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Arguments: None" << std::endl;
        } else {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Arguments" << std::endl;

            for (const auto& arg : n.arguments) {
                std::visit([&indentCount](const auto& expr) {
                    printExpression(expr, indentCount + 2);
                }, *arg);
            }
        }
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
    } else if constexpr (std::is_same_v<NodeType, IfStatement>) {
        std::cout << indent << "If Statement" << std::endl;

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Condition:" << std::endl;
        std::visit([&indentCount](const auto& condition) {
            printExpression(condition, indentCount + 2);
        }, *n.condition);

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Then Block:" << std::endl;
        for (const auto& element : n.thenBlock) {
            std::visit([&indentCount](const auto& item) {
                using ItemType = std::decay_t<decltype(*item)>;

                if constexpr (std::is_same_v<ItemType, Statement>) {
                    std::visit([&indentCount](const auto& expr) {
                        printStatement(expr, indentCount + 2);
                    }, *item);
                } else if constexpr (std::is_same_v<ItemType, Expression>) {
                    std::visit([&indentCount](const auto& expr) {
                        printExpression(expr, indentCount + 2);
                    }, *item);
                } else {
                    std::cout << "Unknown Type in Then Block" << std::endl;
                }
            }, element);
        }

        for (const auto& elseif : n.elseifClauses) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Else If:" << std::endl;
            std::cout << (indent + std::string(SPACE_COUNT * 2, ' ')) << "Condition:" << std::endl;
            std::visit([&indentCount](const auto& condition) {
                printExpression(condition, indentCount + 3);
            }, *elseif.first);

            std::cout << (indent + std::string(SPACE_COUNT * 2, ' ')) << "Block:" << std::endl;
            for (const auto& element : elseif.second) {
                std::visit([&indentCount](const auto& item) {
                    using ItemType = std::decay_t<decltype(*item)>;

                    if constexpr (std::is_same_v<ItemType, Statement>) {
                        std::visit([&indentCount](const auto& expr) {
                            printStatement(expr, indentCount + 3);
                        }, *item);
                    } else if constexpr (std::is_same_v<ItemType, Expression>) {
                        std::visit([&indentCount](const auto& expr) {
                            printExpression(expr, indentCount + 3);
                        }, *item);
                    } else {
                        std::cout << "Unknown Type in Then Block" << std::endl;
                    }
                }, element);
            }
        }

        if (!n.elseBlock.empty()) {
            std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Else Block:" << std::endl;
            for (const auto& element : n.elseBlock) {
                std::visit([&indentCount](const auto& item) {
                    using ItemType = std::decay_t<decltype(*item)>;

                    if constexpr (std::is_same_v<ItemType, Statement>) {
                        std::visit([&indentCount](const auto& expr) {
                            printStatement(expr, indentCount + 2);
                        }, *item);
                    } else if constexpr (std::is_same_v<ItemType, Expression>) {
                        std::visit([&indentCount](const auto& expr) {
                            printExpression(expr, indentCount + 2);
                        }, *item);
                    } else {
                        std::cout << "Unknown Type in Then Block" << std::endl;
                    }
                }, element);
            }
        }
    } else if constexpr (std::is_same_v<NodeType, WhileStatement>) {
        std::cout << indent << "While Statement" << std::endl;

        /**
         * CONDITION
         */

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Condition" << std::endl;
        std::visit([&indentCount](const auto& condition) {
            printStatement(condition, indentCount + 2);
        }, *n.condition);

        /**
         * BLOCK
         */

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Block" << std::endl;
        for (const auto& element : n.block) {
            std::visit([&indentCount](const auto& item) {
                using ItemType = std::decay_t<decltype(*item)>;

                if constexpr (std::is_same_v<ItemType, Statement>) {
                    std::visit([&indentCount](const auto& expr) {
                        printStatement(expr, indentCount + 2);
                    }, *item);
                } else if constexpr (std::is_same_v<ItemType, Expression>) {
                    std::visit([&indentCount](const auto& expr) {
                        printExpression(expr, indentCount + 2);
                    }, *item);
                } else {
                    std::cout << "Unknown Type in Block" << std::endl;
                }
            }, element);
        }
    } else if constexpr (std::is_same_v<NodeType, ForStatement>) {
        std::cout << indent << "For Statement" << std::endl;

        /**
         * INITIALIZATION
         */

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Initialization" << std::endl;
        if (n.initialization.has_value()) {
            std::visit([&indentCount](const auto& stmt) {
                printStatement(stmt, indentCount + 2);
            }, *n.initialization.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT * 2, ' ')) << "None provided" << std::endl;
        }

        /**
         * CONDITION
         */

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Condition" << std::endl;
        if (n.condition.has_value()) {
            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 2);
            }, *n.condition.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT * 2, ' ')) << "None provided" << std::endl;
        }

        /**
         * UPDATE
         */

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Update" << std::endl;
        if (n.update.has_value()) {
            std::visit([&indentCount](const auto& stmt) {
                printStatement(stmt, indentCount + 2);
            }, *n.update.value());
        } else {
            std::cout << (indent + std::string(SPACE_COUNT * 2, ' ')) << "None provided" << std::endl;
        }

        std::cout << (indent + std::string(SPACE_COUNT, ' ')) << "Block" << std::endl;
        for (const auto& element : n.block) {
            std::visit([&indentCount](const auto& item) {
                using ItemType = std::decay_t<decltype(*item)>;

                if constexpr (std::is_same_v<ItemType, Statement>) {
                    std::visit([&indentCount](const auto& expr) {
                        printStatement(expr, indentCount + 2);
                    }, *item);
                } else if constexpr (std::is_same_v<ItemType, Expression>) {
                    std::visit([&indentCount](const auto& expr) {
                        printExpression(expr, indentCount + 2);
                    }, *item);
                } else {
                    std::cout << "Unknown Type in Block" << std::endl;
                }
            }, element);
        }
    } else if constexpr (std::is_same_v<NodeType, BreakStatement>) {
        std::cout << indent << "Break" << std::endl;
    } else if constexpr (std::is_same_v<NodeType, ContinueStatement>) {
        std::cout << indent << "Continue" << std::endl;
    } else if constexpr (std::is_same_v<NodeType, ReturnStatement>) {
        std::cout << indent << "Return" << std::endl;

        if (n.expression.has_value()) {
            std::visit([&indentCount](const auto& expr) {
                printExpression(expr, indentCount + 1);
            }, *n.expression.value());
        }
    } else {
        std::cout << indent << "Unknown Statement Type" << std::endl;
    }
}

void Program::prettyPrint() {
    for (size_t i = 0; i < this->body.size(); i++) {
        const auto& node = this->body[i];

        std::visit([](const auto& ptr) {
            using NodeType = std::decay_t<decltype(*ptr)>;

            if constexpr (std::is_same_v<NodeType, Expression>) {
                std::visit([](const auto& expr) {
                    printExpression(expr, 0);
                }, *ptr);
            } else if constexpr (std::is_same_v<NodeType, Statement>) {
                std::visit([](const auto& expr) {
                    printStatement(expr, 0);
                }, *ptr);
            } else {
                std::cout << "Unknown Node Type" << std::endl;
            }
        }, node);
    }
}
