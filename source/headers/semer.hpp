#ifndef SEMER_HPP
#define SEMER_HPP

#include <string>
#include <vector>
#include "ast.hpp"

enum SemerErrorType {
    SEMANTIC_ERROR
};

inline constexpr std::string_view getSemerErrorTypeString(SemerErrorType type) {
    switch (type) {
        case SemerErrorType::SEMANTIC_ERROR:
            return "Semantic Error";
        default:
            return "Unknown Error Type";
    }
}

enum SemerErrorLevel {
    WARNING,
    ERROR
};

struct SemerError {
    SemerErrorType type;
    SemerErrorLevel level;
    NodeMetadata metadata;
    const std::string& sourceCode;
    std::string message;
    std::string hint;

    SemerError(SemerErrorType type, SemerErrorLevel level, NodeMetadata metadata, const std::string& sourceCode, std::string message, std::string hint)
        : type(type), level(level), metadata(metadata), sourceCode(sourceCode), message(message), hint(hint) {}

    void print() const;
};

class Semer {
private:
    std::vector<SemerError> errors = {};
    const std::string& sourceCode;
    const Program& program;

    template <typename NodeType>
    void analyzeExpression(const NodeType& n);

    template <typename NodeType>
    void analyzeStatement(const NodeType& n);
public:
    Semer(const std::string& sourceCode, const Program& program) : sourceCode(sourceCode), program(program) {};
    ~Semer() {};

    const std::vector<SemerError>& analyze();
};


#endif // SEMER_HPP
