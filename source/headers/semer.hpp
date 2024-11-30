#ifndef SEMER_HPP
#define SEMER_HPP

#include <string>
#include <vector>
#include "ast.hpp"

enum SemerErrorType {
    SEMANTIC_ERROR,
    TYPE_ERROR,
    SYNTAX_ERROR
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

class Scope {
private:
    std::unique_ptr<Scope> parent;
    std::map<std::string, std::shared_ptr<VariableDeclaration>> symbols;
public:
    Scope();
    Scope(std::unique_ptr<Scope> parent);
    ~Scope();

    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;

    void add(std::string name, std::shared_ptr<VariableDeclaration> node);
    const std::shared_ptr<VariableDeclaration> find(const std::string& name) const;
};

class Semer {
private:
    Scope rootScope = Scope();
    std::vector<SemerError> errors = {};
    const std::string& sourceCode;
    const Program& program;

    template <typename T>
    void analyzeExpression(const T& n, Scope& scope);

    template <typename T>
    void analyzeStatement(const T& n, Scope& scope);
public:
    Semer(const std::string& sourceCode, const Program& program);
    ~Semer();

    Semer(const Semer&) = delete;
    Semer& operator=(const Semer&) = delete;

    const std::vector<SemerError>& analyze();
};


#endif // SEMER_HPP
