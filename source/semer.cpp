#include <iostream>
#include <stdexcept>
#include "utils.hpp"
#include "semer.hpp"

std::string getSemerErrorTypeString(SemerErrorType type) {
    switch (type) {
        case SemerErrorType::SEMANTIC_ERROR:
            return "Semantic Error";
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

template <typename T>
void Semer::analyzeExpression(const T& n) {
    throw std::runtime_error("Unknown expression of type : " + std::string(typeid(n).name()));
}

template <typename T>
void Semer::analyzeStatement(const T& n) {
    if constexpr (std::is_same_v<T, VariableDeclaration>) {
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
            using PNodeType = std::decay_t<decltype(*ptr)>;

            if constexpr (std::is_same_v<PNodeType, Expression>) {
                std::visit([&](const auto& expr) {
                    this->analyzeExpression(expr);
                }, *ptr);
            } else if constexpr (std::is_same_v<PNodeType, Statement>) {
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
