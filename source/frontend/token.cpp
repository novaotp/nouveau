#include "token.hpp"

TokenMetadata::TokenMetadata() : column(0), line(0), length(0) {};
TokenMetadata::TokenMetadata(size_t column, size_t line, size_t length) : column(column), line(line), length(length) {};

const NodePosition TokenMetadata::toStartPosition() const {
    return NodePosition(this->column, this->line);
}

const NodePosition TokenMetadata::toEndPosition() const {
    return NodePosition(this->column + this->length, this->line);
}

Token::Token() : type(TokenType::INTEGER), value(""), metadata(TokenMetadata()) {};
Token::Token(TokenType type, std::string value, TokenMetadata metadata) : type(type), value(value), metadata(metadata) {};

std::string getTokenTypeString(const TokenType& type) {
    return tokenTypeToString.at(type);
}
