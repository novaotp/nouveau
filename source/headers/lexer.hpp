#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include "token.hpp"

class Lexer {
private:
    size_t index = 0;
    size_t column = 1;
    size_t line = 1;
    std::string sourceCode;

    /// @brief Advances the current column by 1 and returns the previous column.
    /// @param n The number of column to advance. Defaults to 1.
    /// @return The previous column.
    size_t advanceColumn(size_t n);
    /// @brief Advances the line by 1, resets the column and returns the previous line.
    /// @return The previous line.
    size_t advanceLine();
    /// @brief Advances the index by 1 and returns the previous index.
    /// @param n The number of indexes to advance. Defaults to 1.
    /// @return The previous index.
    size_t advanceIndex(size_t n);
    char getCurrentChar();
    char getNextChar();
    bool isArithmeticOperator(char op);

public:
    Lexer(std::string sourceCode);
    ~Lexer();

    /// @brief Transforms a given source code to an array of tokens.
    /// @return The array of tokens.
    std::vector<Token> tokenize();
};

#endif // LEXER_HPP
