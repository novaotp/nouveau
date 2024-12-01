#include <iostream>
#include <stdexcept>
#include "ast.hpp"
#include "utils.hpp"
#include "semer.hpp"

Scope::Scope() : parent(nullptr) {};
Scope::Scope(std::unique_ptr<Scope> parent) : parent(nullptr) {};
Scope::~Scope() {};

void Scope::add(std::string name, std::shared_ptr<VariableDeclaration> node) {
    this->symbols[name] = std::move(node);
};

const std::shared_ptr<VariableDeclaration> Scope::find(const std::string& name) const {
    if (this->symbols.find(name) != this->symbols.end()) {
        return this->symbols.at(name);
    } else if (this->parent != nullptr) {
        return this->parent->find(name);
    };

    return nullptr;
};

std::string getSemerErrorTypeString(SemerErrorType type) {
    switch (type) {
        case SemerErrorType::SEMANTIC_ERROR:
            return "Semantic Error";
        case SemerErrorType::TYPE_ERROR:
            return "Type Error";
        case SemerErrorType::SYNTAX_ERROR:
            return "Syntax Error";
        default:
            return "Unknown Error Type";
    }
}

SemerError::SemerError(
    SemerErrorType type,
    SemerErrorLevel level,
    NodeMetadata metadata,
    const std::string& sourceCode,
    std::string message,
    std::string hint
) : type(type), level(level), metadata(metadata), sourceCode(sourceCode), message(std::move(message)), hint(std::move(hint)) {};

const std::string SemerError::toString() const {
    std::string result = "";

    auto COLOR = (this->level == SemerErrorLevel::WARNING ? YELLOW : RED);

    result += "\n\tEncountered a " + getSemerErrorTypeString(this->type) + "\n";

    std::vector<std::string> lines = splitStringByNewline(this->sourceCode);
    for (size_t line = this->metadata.start.line; line <= this->metadata.end.line; ++line) {
        result += "\n\t" + std::to_string(line) + " | " + lines.at(line - 1) + "\n\t";

        if (line == this->metadata.start.line) {
            size_t length = line == this->metadata.end.line
                ? this->metadata.end.column - this->metadata.start.column
                : lines[line - 1].size() - this->metadata.start.column;

            result += std::string(this->metadata.start.column + 3, ' ') + COLOR + std::string(length, '~') + RESET;
        } else if (line == this->metadata.end.line) {
            result += std::string(3, ' ') + COLOR + std::string(this->metadata.end.column, '~') + RESET;
        } else {
            result += std::string(3, ' ') + COLOR + std::string(lines[line - 1].size(), '~') + RESET;
        }

        result += "\n";
    }

    result += std::string(COLOR) + "\n\tError: " + message + RESET + "\n";

    if (!hint.empty()) {
        result += "\n\tHint: " + hint + "\n";
    }

    result += "\n";

    return result;
}

Semer::Semer(const std::string& sourceCode, const Program& program) : sourceCode(sourceCode), program(program) {};
Semer::~Semer() {};

std::optional<NodeType> Semer::resolveExpressionReturnType(Expression expr, Scope& scope) {
    return std::visit([&](const auto& e) -> std::optional<NodeType> {
        using ExprType = std::decay_t<decltype(e)>;

        std::optional<NodeType> type = std::nullopt;

        if constexpr (std::is_same_v<ExprType, StringLiteral>) {
            type = std::make_shared<StringType>();
        } else if constexpr (std::is_same_v<ExprType, IntLiteral>) {
            type = std::make_shared<IntegerType>();
        } else if constexpr (std::is_same_v<ExprType, FloatLiteral>) {
            type = std::make_shared<FloatType>();
        } else if constexpr (std::is_same_v<ExprType, BooleanLiteral>) {
            type = std::make_shared<BooleanType>();
        } else if constexpr (std::is_same_v<ExprType, Identifier>) {
            if (scope.find(e.name) != nullptr) {
                type = scope.find(e.name)->type;
            }
        } else if constexpr (std::is_same_v<ExprType, LogicalNotOperation>) {
            std::optional<NodeType> optType = this->resolveExpressionReturnType(*e.expression, scope);

            if (optType.has_value()) {
                type = optType.value();
            }
        } else if constexpr (std::is_same_v<ExprType, BinaryOperation>) {
            std::optional<NodeType> optLeft = this->resolveExpressionReturnType(*e.lhs, scope);
            std::optional<NodeType> optRight = this->resolveExpressionReturnType(*e.rhs, scope);

            if (optLeft.has_value() && optRight.has_value()) {
                NodeType left = optLeft.value();
                NodeType right = optRight.value();

                std::visit([&e, &type](const auto& left, const auto& right) {
                    if (e.op == "==" || e.op == "!=" || e.op == ">" || e.op == "<" || e.op == ">=" || e.op == "<=" || e.op == "&&" || e.op == "||") {
                        type = std::make_shared<BooleanType>();
                    }

                    // Only arithmetic operations are left

                    if (left->compare(std::make_shared<IntegerType>()) && right->compare(std::make_shared<IntegerType>())) {
                        if (e.op == "/") {
                            type = std::make_shared<FloatType>();
                        } else {
                            type = std::make_shared<IntegerType>();
                        }
                    } else if ((left->compare(std::make_shared<IntegerType>()) && right->compare(std::make_shared<FloatType>())) ||
                        (left->compare(std::make_shared<FloatType>()) && right->compare(std::make_shared<IntegerType>())) ||
                        (left->compare(std::make_shared<FloatType>()) && right->compare(std::make_shared<FloatType>()))
                        ) {
                        type = std::make_shared<FloatType>();
                    } else if (e.op == "+" && left->compare(std::make_shared<StringType>()) && right->compare(std::make_shared<StringType>())) {
                        type = left;
                    }
                }, optLeft.value(), optRight.value());
            }
        } else {
            std::cout << "Unsupported Expression Type: " << typeid(ExprType).name() << std::endl;
        }

        return type;
    }, expr);
}

template <typename T>
void Semer::analyzeExpression(const T& n, Scope& scope) {
    if constexpr (std::is_same_v<T, Identifier>) {
        auto node = scope.find(n.name);

        if (node == nullptr) {
            this->errors.push_back(SemerError(
                SemerErrorType::SYNTAX_ERROR,
                SemerErrorLevel::ERROR,
                n.metadata,
                this->sourceCode,
                "'" + n.name + "' is not defined in this scope.",
                "Please define it before using it."
            ));
        }
    } else if constexpr (std::is_same_v<T, LogicalNotOperation>) {
        this->analyzeExpression(*n.expression, scope);
    } else if constexpr (std::is_same_v<T, BinaryOperation>) {
        // TODO
        // * Check if the left is a literal
        // * Yes -> check right
        // * No -> analyze the left

        this->analyzeExpression(*n.lhs, scope);
        this->analyzeExpression(*n.rhs, scope);
    }
}

template <typename T>
void Semer::analyzeStatement(const T& n, Scope& scope) {
    if constexpr (std::is_same_v<T, VariableDeclaration>) {
        if (scope.find(n.identifier) == nullptr) {
            this->errors.push_back(SemerError(
                SemerErrorType::SYNTAX_ERROR,
                SemerErrorLevel::ERROR,
                n.metadata,
                this->sourceCode,
                "'" + n.identifier + "' is already defined in this scope.",
                "Please choose another name or assign to it instead."
            ));
        }

        if (!n.value.has_value()) {
            // ? Currently, we don't support null
            // TODO : after supporting null, remove this

            std::string word = n.isMutable ? "mutable" : "const";

            this->errors.push_back(SemerError(
                SemerErrorType::SEMANTIC_ERROR,
                SemerErrorLevel::ERROR,
                n.metadata,
                this->sourceCode,
                "'" + n.identifier + "' is defined as a " + word + " variable but has no initialization value. This will result in undefined behavior.",
                "Note that 'null' values are not supported yet."
            ));
        } else {
            std::visit([&](auto&& type, const auto& expr) {
                std::optional<NodeType> exprType = this->resolveExpressionReturnType(expr, scope);

                if (exprType.has_value() && !type->compare(exprType.value())) {
                    std::string typeString = type->toString();
                    std::string exprTypeString = std::visit([](const auto& ptr) -> std::string {
                        return ptr->toString();
                    }, exprType.value());

                    this->errors.push_back(SemerError(
                        SemerErrorType::TYPE_ERROR,
                        SemerErrorLevel::ERROR,
                        n.metadata,
                        this->sourceCode,
                        "'" + n.identifier + "' is defined as '" + typeString + "' but received '" + exprTypeString + "'.",
                        "Either change the type of the variable to '" + exprTypeString + "' or change the value to type '" + typeString + "'."
                    ));
                }

                this->analyzeExpression(expr, scope);
            }, n.type, *n.value.value());
        }

        scope.add(n.identifier, std::make_shared<VariableDeclaration>(n));
    } else if constexpr (std::is_same_v<T, VariableAssignment>) {
        if (scope.find(n.identifier) == nullptr) {
            this->errors.push_back(SemerError(
                SemerErrorType::SYNTAX_ERROR,
                SemerErrorLevel::ERROR,
                n.metadata,
                this->sourceCode,
                "'" + n.identifier + "' is not defined in this scope.",
                "Please define it before assigning to it."
            ));
        } else {
            auto node = scope.find(n.identifier);

            std::visit([&](auto&& type, const auto& expr) {
                std::optional<NodeType> exprType = this->resolveExpressionReturnType(expr, scope);

                if (exprType.has_value() && !type->compare(exprType.value())) {
                    std::string typeString = type->toString();
                    std::string exprTypeString = std::visit([](const auto& ptr) -> std::string {
                        return ptr->toString();
                    }, exprType.value());

                    this->errors.push_back(SemerError(
                        SemerErrorType::TYPE_ERROR,
                        SemerErrorLevel::ERROR,
                        n.metadata,
                        this->sourceCode,
                        "'" + n.identifier + "' is defined as '" + typeString + "' but received '" + exprTypeString + "'.",
                        "Either change the type of the variable to '" + exprTypeString + "' or change the value to type '" + typeString + "'."
                    ));
                }

                this->analyzeExpression(expr, scope);
            }, (*node).type, *n.value.value());
        }

        this->analyzeExpression(n.value, scope);
    } else {
        std::cout << RED << "Unknown statement encountered : " << typeid(T).name() << RESET << std::endl;
    }
}

const std::vector<SemerError>& Semer::analyze() {
    for (size_t i = 0; i < this->program.body.size(); i++) {
        const auto& node = this->program.body[i];

        std::visit([&](const auto& ptr) {
            using PNodeType = std::decay_t<decltype(*ptr)>;

            if constexpr (std::is_same_v<PNodeType, Expression>) {
                std::visit([&](const auto& expr) {
                    this->analyzeExpression(expr, this->rootScope);
                }, *ptr);
            } else if constexpr (std::is_same_v<PNodeType, Statement>) {
                std::visit([&](const auto& expr) {
                    this->analyzeStatement(expr, this->rootScope);
                }, *ptr);
            } else {
                throw std::runtime_error("Unknown node type encountered");
            }
        }, node);
    }

    return this->errors;
}
