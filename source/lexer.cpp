#include <map>
#include <stdexcept>
#include "token.hpp"
#include "lexer.hpp"

const size_t TAB_SIZE = 4;

std::map<std::string, TokenType> symbolToTokenType = {
    {"+", TokenType::ADDITION_OPERATOR},
    {";", TokenType::SEMI_COLUMN}};

std::map<char, TokenType> arithmeticOperatorToTokenType = {
    {'+', TokenType::ADDITION_OPERATOR},
    {'-', TokenType::SUBTRACTION_OPERATOR},
    {'*', TokenType::MULTIPLICATION_OPERATOR},
    {'/', TokenType::DIVISION_OPERATOR}};

size_t Lexer::advanceColumn(size_t n = 1)
{
    this->column += 1;
    return this->column - 1;
}

size_t Lexer::advanceLine()
{
    this->line += 1;
    this->column = 1;
    return this->line - 1;
}

size_t Lexer::advanceIndex()
{
    this->index += 1;
    return this->index - 1;
}

char Lexer::getCurrentChar()
{
    return this->sourceCode[this->index];
}

bool Lexer::isArithmeticOperator(char op)
{
    return op == '+' || op == '-' || op == '*' || op == '/';
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens = {};

    while (this->index < sourceCode.size())
    {
        char currentChar = this->getCurrentChar();

        Token token;
        if (isspace(currentChar))
        {
            switch (currentChar)
            {
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
        }
        else if (currentChar == ';')
        {
            token.type = TokenType::SEMI_COLUMN;
            token.value = currentChar;
            token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);

            this->advanceIndex();
        }
        else if (this->isArithmeticOperator(currentChar))
        {
            token.type = arithmeticOperatorToTokenType.at(currentChar);
            token.value = std::string(1, currentChar);
            token.metadata = TokenMetadata(this->column, this->line, 1);

            this->advanceColumn();
            this->advanceIndex();
        }
        else if (isdigit(currentChar))
        {
            std::string value = "";
            bool containsDot = false;
            size_t column_start = this->column;

            while (isdigit(this->getCurrentChar()) || (this->getCurrentChar() == '.' && !containsDot))
            {
                if (this->getCurrentChar() == '.')
                {
                    containsDot = true;
                }

                value += this->getCurrentChar();
                this->advanceColumn();
                this->advanceIndex();
            }

            token.type = containsDot ? TokenType::FLOAT : TokenType::INTEGER;
            token.value = value;
            token.metadata = TokenMetadata(column_start, this->line, value.size());
        }
        else if (currentChar == '"')
        {
            std::string value = "";
            size_t column_start = this->column;

            // Skip the first "
            this->advanceColumn();
            this->advanceIndex();

            while (this->getCurrentChar() != '"')
            {
                value += this->getCurrentChar();
                this->advanceColumn();
                this->advanceIndex();
            }

            // Skip the last "
            this->advanceColumn();
            this->advanceIndex();

            token.type = TokenType::STRING;
            token.value = value;
            token.metadata = TokenMetadata(column_start, this->line, value.size() + 2);
        }
        else if (isalpha(currentChar))
        {
            std::string value = "";
            size_t columnStart = this->column;

            while (isalnum(this->getCurrentChar()) || this->getCurrentChar() == '_')
            {
                value += this->getCurrentChar();

                this->advanceColumn();
                this->advanceIndex();
            }

            token.value = value;
            token.metadata = TokenMetadata(columnStart, this->line, value.size());
            if (value == "true" || value == "false")
            {
                token.type = TokenType::BOOLEAN;
            }
            else
            {
                token.type = TokenType::IDENTIFIER;
            }
        }
        else
        {
            throw std::runtime_error("Error: Unknown character encountered <" + std::string(1, currentChar) + ">");
        }

        tokens.push_back(token);
    }

    tokens.push_back(Token(TokenType::END_OF_FILE, "", TokenMetadata(this->advanceColumn(), this->line, 0)));

    return tokens;
}
