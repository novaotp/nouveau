#include <iostream>
#include <stdexcept>
#include "utils.hpp"
#include "semer.hpp"

void SemerError::print() const {
    auto COLOR = (this->level == SemerErrorLevel::WARNING ? YELLOW : RED);

    std::cout << std::endl;
    std::cout << "\tEncountered a " << getSemerErrorTypeString(this->type) << std::endl;

    std::vector<std::string> lines = splitStringByNewline(this->sourceCode);
    for (size_t line = this->metadata.start.line; line <= this->metadata.end.line; ++line) {
        std::cout << "\n\t" << line << " | " << lines.at(line - 1) << "\n\t";

        if (line == this->metadata.start.line) {
            std::cout << std::string(this->metadata.start.column + 3, ' ')
                << COLOR << std::string(line == this->metadata.end.line
                    ? this->metadata.end.column - this->metadata.start.column
                    : lines[line - 1].size() - this->metadata.start.column, '~') << RESET;
        } else if (line == this->metadata.end.line) {
            std::cout << std::string(3, ' ') << COLOR << std::string(this->metadata.end.column, '~') << RESET;
        } else {
            std::cout << std::string(3, ' ') << COLOR << std::string(lines[line - 1].size(), '~') << RESET;
        }

        std::cout << std::endl;
    }

    std::cout << COLOR << "\n\tError: " << message << RESET << std::endl;

    if (!hint.empty()) {
        std::cout << "\n\tHint: " << hint << std::endl;
    }

    std::cout << std::endl;
}

template <typename NodeType>
void Semer::analyzeExpression(const NodeType& n) {
    throw std::runtime_error("Unknown expression");
}

template <typename NodeType>
void Semer::analyzeStatement(const NodeType& n) {
    if constexpr (std::is_same_v<NodeType, VariableDeclaration>) {
        if (!n.isMutable && !n.value.has_value()) {
            this->errors.push_back(SemerError(
                SemerErrorType::SEMANTIC_ERROR,
                SemerErrorLevel::WARNING,
                n.metadata,
                this->sourceCode,
                "'" + n.identifier + "' is defined as a const but has no initialization value. This will result in undefined behavior.",
                "Either set an initialization value or set the variable as mutable."
            ));
        }
    } else {
        throw std::runtime_error("Unknown statement");
    }
}

const std::vector<SemerError>& Semer::analyze() {
    for (size_t i = 0; i < this->program.body.size(); i++) {
        const auto& node = this->program.body[i];

        std::visit([&](const auto& ptr) {
            using NodeType = std::decay_t<decltype(*ptr)>;

            if constexpr (std::is_same_v<NodeType, Expression>) {
                std::visit([&](const auto& expr) {
                    this->analyzeExpression(expr);
                }, *ptr);
            } else if constexpr (std::is_same_v<NodeType, Statement>) {
                std::visit([&](const auto& expr) {
                    this->analyzeStatement(expr);
                }, *ptr);
            } else {
                throw std::runtime_error("Unknown node type encountered");
            }
        }, node);
    }

    return this->errors;
}
