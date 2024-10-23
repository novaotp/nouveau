#include <map>
#include <stdexcept>
#include "token.h"
#include "lexer.h"

const size_t TAB_SIZE = 4;

std::map<std::string, TokenType> symbolToTokenType = {
    {"+", TokenType::ADDITION_OPERATOR},
    {";", TokenType::SEMI_COLUMN}};

std::map<char, TokenType> arithmeticOperatorToTokenType = {
    {'+', TokenType::ADDITION_OPERATOR},
    {'-', TokenType::SUBSTRACTION_OPERATOR},
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

bool Lexer::isNumber(char num)
{
    return isdigit(num);
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
        else if (this->isNumber(currentChar))
        {
            std::string value = "";
            bool containsDot = false;

            while (this->isNumber(this->getCurrentChar()) || (this->getCurrentChar() == '.' && !containsDot))
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
            token.metadata = TokenMetadata(this->column, this->line, value.size());
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
