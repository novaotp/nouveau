#include <map>
#include <stdexcept>
#include "token.hpp"
#include "lexer.hpp"

// ? How to handle other tab sizes properly
const size_t TAB_SIZE = 4;

const std::map<std::string, TokenType> arithmeticOperatorToTokenType = {
    { "+", TokenType::ADDITION_OPERATOR },
    { "-", TokenType::SUBTRACTION_OPERATOR },
    { "*", TokenType::MULTIPLICATION_OPERATOR },
    { "/", TokenType::DIVISION_OPERATOR },
    { "%", TokenType::MODULO_OPERATOR },
    { "+=", TokenType::ADDITION_ASSIGNMENT_OPERATOR },
    { "-=", TokenType::SUBTRACTION_ASSIGNMENT_OPERATOR },
    { "*=", TokenType::MULTIPLICATION_ASSIGNMENT_OPERATOR },
    { "/=", TokenType::DIVISION_ASSIGNMENT_OPERATOR },
    { "%=", TokenType::MODULO_ASSIGNMENT_OPERATOR }
};

const std::map<char, TokenType> punctuationToTokenType = {
    { ';', TokenType::SEMI_COLON },
    { '!', TokenType::EXCLAMATION_MARK }
};

const std::map<std::string, TokenType> comparisonOperatorToTokenType = {
    { "==", TokenType::EQUAL_OPERATOR },
    { "!=", TokenType::NOT_EQUAL_OPERATOR },
    { ">=", TokenType::GREATER_OR_EQUAL_OPERATOR },
    { ">", TokenType::GREATER_THAN_OPERATOR },
    { "<=", TokenType::LESS_OR_EQUAL_OPERATOR },
    { "<", TokenType::LESS_THAN_OPERATOR }
};

const std::map<std::string, TokenType> keywordToTokenType = {
    { "true", TokenType::BOOLEAN },
    { "false", TokenType::BOOLEAN },
    { "string", TokenType::TYPE },
    { "int", TokenType::TYPE },
    { "float", TokenType::TYPE },
    { "bool", TokenType::TYPE },
    { "void", TokenType::TYPE },
    { "mut", TokenType::MUTABLE_KEYWORD }
};

Lexer::Lexer(std::string sourceCode) : sourceCode(sourceCode) {};
Lexer::~Lexer() {};

size_t Lexer::advanceColumn(size_t n = 1) {
    this->column += n;
    return this->column - n;
}

size_t Lexer::advanceLine() {
    this->line += 1;
    this->column = 1;
    return this->line - 1;
}

size_t Lexer::advanceIndex(size_t n = 1) {
    this->index += n;
    return this->index - n;
}

char Lexer::getCurrentChar() {
    return this->sourceCode[this->index];
}

char Lexer::getNextChar() {
    return this->sourceCode[this->index + 1];
}

bool Lexer::isArithmeticOperator(char op) {
    return op == '+' || op == '-' || op == '*' || op == '/' || op == '%';
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens = {};

    while (this->index < this->sourceCode.size()) {
        char currentChar = this->getCurrentChar();

        Token token;
        if (isspace(currentChar)) {
            switch (currentChar) {
                case '\n':
                    this->advanceLine();
                    break;
                case '\t':
                    this->advanceColumn(TAB_SIZE);
                    break;
                default:
                    this->advanceColumn();
                    break;
            }

            this->advanceIndex();
            continue;
        } else if (currentChar == '/' && this->getCurrentChar() == '/' && this->sourceCode[this->index + 2] == '/') {
            while (this->getCurrentChar() != '\n' && this->index < this->sourceCode.size()) {
                this->advanceColumn();
                this->advanceIndex();
            }

            continue;
        } else if ((((currentChar == '=') || (currentChar == '!')) && (this->getNextChar() == '=')) || currentChar == '>' || currentChar == '<') {
            std::string op = std::string(1, this->getCurrentChar()) + std::string(1, this->getNextChar());

            if (comparisonOperatorToTokenType.find(op) != comparisonOperatorToTokenType.end()) {
                token.type = comparisonOperatorToTokenType.at(op);
                token.value = op;
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);
                this->advanceColumn();
                this->advanceIndex(2);
            } else if (comparisonOperatorToTokenType.find(std::string(1, currentChar)) != comparisonOperatorToTokenType.end()) {
                token.type = comparisonOperatorToTokenType.at(std::string(1, currentChar));
                token.value = std::string(1, currentChar);
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);
                this->advanceIndex();
            }
        } else if ((currentChar == '&' && this->getNextChar() == '&') || (currentChar == '|' && this->getNextChar() == '|')) {
            if (currentChar == '&') {
                token.type = TokenType::AND_OPERATOR;
                token.value = "&&";
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);
                this->advanceColumn();
                this->advanceIndex(2);
            } else if (currentChar == '|') {
                token.type = TokenType::OR_OPERATOR;
                token.value = "||";
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);
                this->advanceColumn();
                this->advanceIndex(2);
            }
        } else if (punctuationToTokenType.find(currentChar) != punctuationToTokenType.end()) {
            token.type = punctuationToTokenType.at(currentChar);
            token.value = currentChar;
            token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);

            this->advanceIndex();
        } else if (currentChar == '=') {
            token.type = TokenType::ASSIGNMENT_OPERATOR;
            token.value = currentChar;
            token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);
            this->advanceIndex();
        } else if (this->isArithmeticOperator(currentChar)) {
            if (this->getNextChar() == '=') {
                std::string op = std::string(1, currentChar) + std::string(1, this->getNextChar());

                token.type = arithmeticOperatorToTokenType.at(op);
                token.value = op;
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);

                this->advanceColumn();
                this->advanceIndex(2);
            } else {
                std::string equalOperator = std::string(1, currentChar);

                token.type = arithmeticOperatorToTokenType.at(equalOperator);
                token.value = equalOperator;
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);

                this->advanceIndex();
            }
        } else if (isdigit(currentChar)) {
            std::string value = "";
            bool containsDot = false;
            size_t columnStart = this->column;

            while (isdigit(this->getCurrentChar()) || (this->getCurrentChar() == '.' && !containsDot)) {
                if (this->getCurrentChar() == '.') {
                    containsDot = true;
                }

                value += this->getCurrentChar();
                this->advanceColumn();
                this->advanceIndex();
            }

            token.type = containsDot ? TokenType::FLOAT : TokenType::INTEGER;
            token.value = value;
            token.metadata = TokenMetadata(columnStart, this->line, value.size());
        } else if (currentChar == '"') {
            std::string value = "";
            size_t columnStart = this->column;

            // Skip the first "
            this->advanceColumn();
            this->advanceIndex();

            while (this->getCurrentChar() != '"') {
                value += this->getCurrentChar();
                this->advanceColumn();
                this->advanceIndex();
            }

            // Skip the last "
            this->advanceColumn();
            this->advanceIndex();

            token.type = TokenType::STRING;
            token.value = value;
            token.metadata = TokenMetadata(columnStart, this->line, value.size() + 2);
        } else if (isalpha(currentChar)) {
            std::string value = "";
            size_t columnStart = this->column;

            while (isalnum(this->getCurrentChar()) || this->getCurrentChar() == '_') {
                value += this->getCurrentChar();

                this->advanceColumn();
                this->advanceIndex();
            }

            token.value = value;
            token.metadata = TokenMetadata(columnStart, this->line, value.size());

            if (keywordToTokenType.find(value) != keywordToTokenType.end()) {
                token.type = keywordToTokenType.at(value);
            } else {
                token.type = TokenType::IDENTIFIER;
            }
        } else {
            throw std::runtime_error("Error: Unknown character encountered <" + std::string(1, currentChar) + ">");
        }

        tokens.push_back(token);
    }

    tokens.push_back(Token(TokenType::END_OF_FILE, "", TokenMetadata(this->advanceColumn(), this->line, 0)));

    return tokens;
}
