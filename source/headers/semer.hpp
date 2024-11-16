#ifndef SEMER_HPP
#define SEMER_HPP

#include <string>
#include <vector>
#include "ast.hpp"

enum SemerErrorType {
    SEMANTIC_ERROR
};

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

    SemerError(SemerErrorType type, SemerErrorLevel level, NodeMetadata metadata, const std::string& sourceCode, std::string message, std::string hint);

    const std::string toString() const;
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
    Semer(const std::string& sourceCode, const Program& program);
    ~Semer();

    const std::vector<SemerError>& analyze();
};


#endif // SEMER_HPP
