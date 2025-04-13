#include <iostream>
#include <stdexcept>
#include "ast.hpp"
#include "utils.hpp"
#include "semer.hpp"

Symbol::Symbol(std::shared_ptr<VariableDeclaration> value) : value(value) {};

Scope::Scope() {};
Scope::Scope(std::shared_ptr<Scope> parent) : parent(parent) {};
Scope::~Scope() {};

void Scope::addSymbol(std::string name, std::shared_ptr<VariableDeclaration> node) {
    this->symbols[name] = std::make_shared<Symbol>(Symbol(std::move(node)));
};

std::shared_ptr<Scope> Scope::addScope() {
    auto scope = std::make_shared<Scope>();
    this->scopes.push_back(scope);

    return scope;
};

const std::shared_ptr<Symbol> Scope::find(const std::string& name) const {
    if (this->symbols.find(name) != this->symbols.end()) {
        return this->symbols.at(name);
    } else if (this->parent != nullptr) {
        return this->parent->find(name);
    };

    return nullptr;
};

void Scope::printSymbolTable() {
    for (const auto& [identifier, symbol] : this->symbols) {
        std::cout << identifier << " was referenced " << symbol->referenceCount << " times." << std::endl;
    }
}

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

    std::string word = "";
    if (this->level == SemerErrorLevel::WARNING) {
        word = "Warning";
    } else {
        word = "Error";
    }

    result += std::string(COLOR) + "\n\t" + word + ": " + message + RESET + "\n";

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
                type = scope.find(e.name)->value->type;
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
                    if (
                    e.op == BinaryOperator::EQUAL ||
                    e.op == BinaryOperator::NOT_EQUAL ||
                    e.op == BinaryOperator::GREATER_THAN ||
                    e.op == BinaryOperator::LESS_THAN ||
                    e.op == BinaryOperator::GREATER_OR_EQUAL ||
                    e.op == BinaryOperator::LESS_OR_EQUAL ||
                    e.op == BinaryOperator::AND ||
                    e.op == BinaryOperator::OR
                    ) {
                        type = std::make_shared<BooleanType>();
                    }

                    // Only arithmetic operations are left

                    if (left->compare(std::make_shared<IntegerType>()) && right->compare(std::make_shared<IntegerType>())) {
                        if (e.op == BinaryOperator::DIVISION) {
                            type = std::make_shared<FloatType>();
                        } else {
                            type = std::make_shared<IntegerType>();
                        }
                    } else if ((left->compare(std::make_shared<IntegerType>()) && right->compare(std::make_shared<FloatType>())) ||
                               (left->compare(std::make_shared<FloatType>()) && right->compare(std::make_shared<IntegerType>())) ||
                               (left->compare(std::make_shared<FloatType>()) && right->compare(std::make_shared<FloatType>()))
                              ) {
                        // ? Need to rework this, but how
                        // * Problem : float * 0 -> should be int
                        // * However, type returns as float

                        type = std::make_shared<FloatType>();
                    } else if (
                    e.op == BinaryOperator::ADDITION &&
                            left->compare(std::make_shared<StringType>()) &&
                            right->compare(std::make_shared<StringType>())
                    ) {
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

std::string Semer::resolveExpressionReturnTypeString(Expression expr, Scope& scope) {
    return std::visit([](const auto& value) -> std::string {
        return value->toString();
    }, this->resolveExpressionReturnType(expr, scope).value());
}

template <typename T>
void Semer::analyzeBinaryOperation(const T& n, Scope& scope) {
    std::visit([&](const auto& left, const auto& right) {
        using LeftType = std::decay_t<decltype(left)>;
        using RightType = std::decay_t<decltype(right)>;

        if constexpr ((std::is_same_v<LeftType, StringLiteral> ||
                       std::is_same_v<LeftType, IntLiteral> ||
                       std::is_same_v<LeftType, FloatLiteral> ||
                       std::is_same_v<LeftType, BooleanLiteral>) && (std::is_same_v<RightType, StringLiteral> ||
                               std::is_same_v<RightType, IntLiteral> ||
                               std::is_same_v<RightType, FloatLiteral> ||
                               std::is_same_v<RightType, BooleanLiteral>)) {
            if constexpr (std::is_same_v<LeftType, StringLiteral> && std::is_same_v<RightType, StringLiteral>) {
                if (
                n.op == BinaryOperator::SUBTRACTION ||
                n.op == BinaryOperator::MULTIPLICATION ||
                n.op == BinaryOperator::DIVISION ||
                n.op == BinaryOperator::MODULO
                ) {
                    this->errors.push_back(SemerError(
                                               SemerErrorType::SYNTAX_ERROR,
                                               SemerErrorLevel::ERROR,
                                               n.metadata,
                                               this->sourceCode,
                                               "Cannot perform '" + binaryOperatorToString(n.op) + "' operation on strings.",
                                               "Please use a valid operator for strings."
                                           ));
                }
            } else if (std::is_same_v<LeftType, StringLiteral> && !(n.op == BinaryOperator::AND || n.op == BinaryOperator::OR)) {
                // * Strings can only perform '&&' and '||' operations with other types

                this->errors.push_back(SemerError(
                                           SemerErrorType::SYNTAX_ERROR,
                                           SemerErrorLevel::ERROR,
                                           n.metadata,
                                           this->sourceCode,
                                           "Can only perform '&&' and '||' operations between 'string' and '" + this->resolveExpressionReturnTypeString(right, scope) + "'.",
                                           "Please use a valid operator for strings."
                                       ));
            } else if (
                (std::is_same_v<LeftType, IntLiteral> ||
                 std::is_same_v<LeftType, FloatLiteral>) &&
                !(std::is_same_v<RightType, IntLiteral> ||
                  std::is_same_v<RightType, FloatLiteral>) && // * Numbers can perform any operations with other numbers
            !(n.op == BinaryOperator::AND || n.op == BinaryOperator::OR)
            ) {
                // * Numbers can only perform '&&' and '||' operations with other types

                this->errors.push_back(SemerError(
                                           SemerErrorType::SYNTAX_ERROR,
                                           SemerErrorLevel::ERROR,
                                           n.metadata,
                                           this->sourceCode,
                                           "Can only perform '&&' and '||' operations between 'number' and '" + this->resolveExpressionReturnTypeString(right, scope)+ "'.",
                                           "Please use a valid operator for numbers."
                                       ));
            } else if (std::is_same_v<LeftType, BooleanLiteral> && !(n.op == BinaryOperator::AND || n.op == BinaryOperator::OR)) {
                // * Booleans can only perform '&&' and '||' operations with booleans and other types

                this->errors.push_back(SemerError(
                                           SemerErrorType::SYNTAX_ERROR,
                                           SemerErrorLevel::ERROR,
                                           n.metadata,
                                           this->sourceCode,
                                           "Can only perform '&&' and '||' operations between 'bool' and '" + this->resolveExpressionReturnTypeString(right, scope) + "'.",
                                           "Please use a valid operator for booleans."
                                       ));
            }
        } else {
            if constexpr (!(std::is_same_v<LeftType, StringLiteral> ||
                            std::is_same_v<LeftType, IntLiteral> ||
                            std::is_same_v<LeftType, FloatLiteral> ||
                            std::is_same_v<LeftType, BooleanLiteral>)) {
                this->analyzeExpression(left, scope);
            }

            if constexpr (!(std::is_same_v<RightType, StringLiteral> ||
                            std::is_same_v<RightType, IntLiteral> ||
                            std::is_same_v<RightType, FloatLiteral> ||
                            std::is_same_v<RightType, BooleanLiteral>)) {
                this->analyzeExpression(right, scope);
            }

            std::optional<NodeType> leftReturnType = this->resolveExpressionReturnType(left, scope);
            std::optional<NodeType> rightReturnType = this->resolveExpressionReturnType(right, scope);

            if (!leftReturnType.has_value() || !rightReturnType.has_value()) {
                this->errors.push_back(SemerError(
                                           SemerErrorType::SYNTAX_ERROR,
                                           SemerErrorLevel::ERROR,
                                           n.metadata,
                                           this->sourceCode,
                                           "Cannot perform '" + binaryOperatorToString(n.op) + "' operation on between these values.",
                                           "Please use a valid operator."
                                       ));
            }

            // ? What to do next
        }
    }, *n.lhs, *n.rhs);
};

template <typename T>
void Semer::analyzeExpression(const T& n, Scope& scope) {
    if constexpr (std::is_same_v<T, Identifier>) {
        auto symbol = scope.find(n.name);

        if (symbol == nullptr) {
            this->errors.push_back(SemerError(
                                       SemerErrorType::SYNTAX_ERROR,
                                       SemerErrorLevel::ERROR,
                                       n.metadata,
                                       this->sourceCode,
                                       "'" + n.name + "' is not defined in this scope.",
                                       "Please define it before using it."
                                   ));
        } else {
            symbol->referenceCount++;
        }
    } else if constexpr (std::is_same_v<T, LogicalNotOperation>) {
        this->analyzeExpression(*n.expression, scope);
    } else if constexpr (std::is_same_v<T, BinaryOperation>) {
        this->analyzeBinaryOperation(n, scope);
    }
}

template <typename T>
void Semer::analyzeStatement(const T& n, Scope& scope) {
    if constexpr (std::is_same_v<T, VariableDeclaration>) {
        if (scope.find(n.identifier) != nullptr) {
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

        scope.addSymbol(n.identifier, std::make_shared<VariableDeclaration>(n));
    } else if constexpr (std::is_same_v<T, VariableAssignment>) {
        auto symbol = scope.find(n.identifier);

        if (symbol == nullptr) {
            this->errors.push_back(SemerError(
                                       SemerErrorType::SYNTAX_ERROR,
                                       SemerErrorLevel::ERROR,
                                       n.metadata,
                                       this->sourceCode,
                                       "'" + n.identifier + "' is not defined in this scope.",
                                       "Please define it before assigning to it."
                                   ));
        } else {
            auto node = symbol->value;

            if (!node->isMutable) {
                this->errors.push_back(SemerError(
                                           SemerErrorType::SEMANTIC_ERROR,
                                           SemerErrorLevel::ERROR,
                                           n.metadata,
                                           this->sourceCode,
                                           "'" + n.identifier + "' is declared as a constant but you are trying to assign to it.",
                                           "Declare it as mutable if you need to assign to it."
                                       ));
            }

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
            }, node->type, *n.value);
        }

        this->analyzeExpression(n.value, scope);
    } else {
        std::cout << RED << "Unknown statement encountered : " << typeid(T).name() << RESET << std::endl;
    }
}

void Semer::warnUnusedSymbols(std::shared_ptr<Scope> scope) {
    for (const auto& [identifier, symbol] : scope->symbols) {
        if (symbol->referenceCount == 0) {
            this->errors.push_back(SemerError(
                                       SemerErrorType::SEMANTIC_ERROR,
                                       SemerErrorLevel::WARNING,
                                       symbol->value->metadata,
                                       this->sourceCode,
                                       "'" + symbol->value->identifier + "' is declared but never used. Did you forget to use it ?",
                                       "Remove unused code to improve performance."
                                   ));
        }
    }

    for (size_t i = 0; i < scope->scopes.size(); i++)
    {
        this->warnUnusedSymbols(scope->scopes.at(i));
    }
};

std::tuple<std::vector<SemerError>&, std::shared_ptr<Scope>> Semer::analyze() {
    for (size_t i = 0; i < this->program.body.size(); i++) {
        const auto& node = this->program.body[i];

        std::visit([&](const auto& ptr) {
            using StatementOrExpression = std::decay_t<decltype(*ptr)>;

            if constexpr (std::is_same_v<StatementOrExpression, Expression>) {
                std::visit([&](const auto& expr) {
                    this->analyzeExpression(expr, this->rootScope);
                }, *ptr);
            } else if constexpr (std::is_same_v<StatementOrExpression, Statement>) {
                std::visit([&](const auto& expr) {
                    this->analyzeStatement(expr, this->rootScope);
                }, *ptr);
            } else {
                throw std::runtime_error("Unknown node type encountered");
            }
        }, node);
    }

    this->warnUnusedSymbols(std::make_shared<Scope>(this->rootScope));

    return {this->errors, std::make_shared<Scope>(this->rootScope)};
}
