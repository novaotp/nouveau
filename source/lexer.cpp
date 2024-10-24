#include <map>
#include <stdexcept>
#include "token.hpp"
#include "lexer.hpp"

// ? How to handle other tab sizes properly
const size_t TAB_SIZE = 4;

std::map<std::string, TokenType> arithmeticOperatorToTokenType = {
    {"+", TokenType::ADDITION_OPERATOR},
    {"-", TokenType::SUBTRACTION_OPERATOR},
    {"*", TokenType::MULTIPLICATION_OPERATOR},
    {"/", TokenType::DIVISION_OPERATOR},
    {"+=", TokenType::ADDITION_ASSIGNMENT_OPERATOR},
    {"-=", TokenType::SUBTRACTION_ASSIGNMENT_OPERATOR},
    {"*=", TokenType::MULTIPLICATION_ASSIGNMENT_OPERATOR},
    {"/=", TokenType::DIVISION_ASSIGNMENT_OPERATOR}};

// ? Better naming
std::map<char, TokenType> parenthesisToTokenType = {
    {'(', TokenType::LEFT_PARENTHESIS},
    {')', TokenType::RIGHT_PARENTHESIS},
    {'[', TokenType::LEFT_BRACKET},
    {']', TokenType::RIGHT_BRACKET},
    {'{', TokenType::LEFT_BRACE},
    {'}', TokenType::RIGHT_BRACE}};

std::map<char, TokenType> punctuationToTokenType = {
    {',', TokenType::COMMA},
    {'.', TokenType::DOT},
    {':', TokenType::COLON},
    {';', TokenType::SEMI_COLON}};

size_t Lexer::advanceColumn(size_t n = 1)
{
    this->column += n;
    return this->column - n;
}

size_t Lexer::advanceLine()
{
    this->line += 1;
    this->column = 1;
    return this->line - 1;
}

size_t Lexer::advanceIndex(size_t n = 1)
{
    this->index += n;
    return this->index - n;
}

char Lexer::getCurrentChar()
{
    return this->sourceCode[this->index];
}

char Lexer::getNextChar()
{
    return this->sourceCode[this->index + 1];
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
        else if (currentChar == '/' && this->getCurrentChar() == '/' && this->sourceCode[this->index + 2] == '/')
        {
            while (this->getCurrentChar() != '\n' && this->index < this->sourceCode.size())
            {
                this->advanceColumn();
                this->advanceIndex();
            }

            continue;
        }
        else if (punctuationToTokenType.find(currentChar) != punctuationToTokenType.end())
        {
            token.type = punctuationToTokenType.at(currentChar);
            token.value = currentChar;
            token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);

            this->advanceIndex();
        }
        else if (parenthesisToTokenType.find(currentChar) != parenthesisToTokenType.end())
        {
            token.type = parenthesisToTokenType.at(currentChar);
            token.value = currentChar;
            token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);

            this->advanceIndex();
        }
        else if ((((currentChar == '=') || (currentChar == '!')) && (this->getNextChar() == '=')) || currentChar == '>' || currentChar == '<')
        {
            if (currentChar == '=')
            {
                token.type = TokenType::EQUAL_OPERATOR;
                token.value = "==";
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);

                this->advanceColumn();
                this->advanceIndex(2);
            }
            else if (currentChar == '!')
            {
                token.type = TokenType::NOT_EQUAL_OPERATOR;
                token.value = "!=";
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);

                this->advanceColumn();
                this->advanceIndex(2);
            }
            else
            {
                if ((currentChar == '>') && (this->getNextChar() == '='))
                {
                    token.type = TokenType::GREATER_OR_EQUAL_OPERATOR;
                    token.value = ">=";
                    token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);

                    this->advanceColumn();
                    this->advanceIndex(2);
                }
                else if ((currentChar == '<') && (this->getNextChar() == '='))
                {
                    token.type = TokenType::LESS_OR_EQUAL_OPERATOR;
                    token.value = "<=";
                    token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);

                    this->advanceColumn();
                    this->advanceIndex(2);
                }
                else if (currentChar == '>')
                {
                    token.type = TokenType::GREATER_THAN_OPERATOR;
                    token.value = ">";
                    token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);
                    this->advanceIndex();
                }
                else
                {
                    token.type = TokenType::LESS_THAN_OPERATOR;
                    token.value = "<";
                    token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);
                    this->advanceIndex();
                }
            }
        }
        else if (currentChar == '!' || (currentChar == '&' && this->getNextChar() == '&') || (currentChar == '|' && this->getNextChar() == '|'))
        {
            if (currentChar == '!')
            {
                token.type = TokenType::NOT_OPERATOR;
                token.value = "!";
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);
                this->advanceIndex();
            }
            else if (currentChar == '&' && this->getNextChar() == '&')
            {
                token.type = TokenType::AND_OPERATOR;
                token.value = "&&";
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);
                this->advanceColumn();
                this->advanceIndex(2);
            }
            else if (currentChar == '|' && this->getNextChar() == '|')
            {
                token.type = TokenType::OR_OPERATOR;
                token.value = "||";
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);
                this->advanceColumn();
                this->advanceIndex(2);
            }
        }
        else if (currentChar == '=')
        {
            token.type = TokenType::ASSIGNMENT_OPERATOR;
            token.value = currentChar;
            token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);
            this->advanceIndex();
        }
        else if (this->isArithmeticOperator(currentChar))
        {
            if (this->getNextChar() == '=')
            {
                std::string op = std::string(1, currentChar) + std::string(1, this->getNextChar());

                token.type = arithmeticOperatorToTokenType.at(op);
                token.value = op;
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 2);

                this->advanceColumn();
                this->advanceIndex(2);
            }
            else
            {
                std::string equalOperator = std::string(1, currentChar);

                token.type = arithmeticOperatorToTokenType.at(equalOperator);
                token.value = equalOperator;
                token.metadata = TokenMetadata(this->advanceColumn(), this->line, 1);

                this->advanceIndex();
            }
        }
        else if (isdigit(currentChar))
        {
            std::string value = "";
            bool containsDot = false;
            size_t columnStart = this->column;

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
            token.metadata = TokenMetadata(columnStart, this->line, value.size());
        }
        else if (currentChar == '"')
        {
            std::string value = "";
            size_t columnStart = this->column;

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
            token.metadata = TokenMetadata(columnStart, this->line, value.size() + 2);
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
            else if (value == "string" || value == "int" || value == "float" || value == "bool")
            {
                token.type = TokenType::TYPE;
            }
            else if (value == "fn")
            {
                token.type = TokenType::FUNCTION_KEYWORD;
            }
            else if (value == "mut")
            {
                token.type = TokenType::MUTABLE_KEYWORD;
            }
            else if (value == "const")
            {
                token.type = TokenType::CONST_KEYWORD;
            }
            else if (value == "if")
            {
                token.type = TokenType::IF_KEYWORD;
            }
            else if (value == "else")
            {
                token.type = TokenType::ELSE_KEYWORD;

                // ? There could be more spaces between the else and the if
                if (this->getCurrentChar() == ' ' && this->getNextChar() == 'i' && this->sourceCode[this->index + 2] == 'f')
                {
                    token.value += " if";
                    token.type = TokenType::ELSE_IF_KEYWORD;
                    token.metadata.length += 3;

                    this->advanceColumn(3);
                    this->advanceIndex(3);
                }
            }
            else if (value == "for")
            {
                token.type = TokenType::FOR_KEYWORD;
            }
            else if (value == "while")
            {
                token.type = TokenType::WHILE_KEYWORD;
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
