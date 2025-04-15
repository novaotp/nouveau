#ifndef SEMER_HPP
#define SEMER_HPP

#include <string>
#include <vector>
#include <map>
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

struct Symbol {
    std::shared_ptr<VariableDeclaration> value;
    size_t referenceCount = 0;

    Symbol(std::shared_ptr<VariableDeclaration> value);
};

class Scope {
private:
    std::shared_ptr<Scope> parent;

public:
    std::map<std::string, std::shared_ptr<Symbol>> symbols = {};
    std::vector<std::shared_ptr<Scope>> scopes = {};

    Scope();
    Scope(std::shared_ptr<Scope> parent);
    ~Scope();

    void addSymbol(std::string name, std::shared_ptr<VariableDeclaration> node);

    /// @brief Adds a new scope that is linked to the current one.
    /// @return The newly created scope.
    std::shared_ptr<Scope> addScope();

    const std::shared_ptr<Symbol> find(const std::string& name) const;

    void printSymbolTable();
};

class Semer {
private:
    Scope rootScope = Scope();
    std::vector<SemerError> errors = {};
    const std::string& sourceCode;
    const Program& program;

    /// @brief Resolves the return type of an expression.
    /// @param expr The expression to resolve.
    /// @param scope The current scope.
    /// @return A `NodeType` if the expression has a valid return type, otherwise `std::nullopt`.
    std::optional<NodeType> resolveExpressionReturnType(Expression expr, Scope& scope);

    std::string resolveExpressionReturnTypeString(Expression expr, Scope& scope);

    /* /// @brief Checks if the given type is either an `IntegerType` or a `FloatType`.
    static constexpr bool isNumberType(const NodeType& type) {
        return std::visit([](const auto& t) -> bool {
            return t->compare(std::make_shared<IntegerType>()) || t->compare(std::make_shared<FloatType>());
        }, type);
    } */

    template <typename T>
    void analyzeBinaryOperation(const T& n, Scope& scope);

    template <typename T>
    void analyzeExpression(const T& n, Scope& scope);

    template <typename T>
    void analyzeStatement(const T& n, Scope& scope);

    void warnUnusedSymbols(std::shared_ptr<Scope> scope);

public:
    Semer(const std::string& sourceCode, const Program& program);
    ~Semer();

    Semer(const Semer&) = delete;
    Semer& operator=(const Semer&) = delete;

    std::tuple<std::vector<SemerError>&, std::shared_ptr<Scope>> analyze();
};

#endif // SEMER_HPP
