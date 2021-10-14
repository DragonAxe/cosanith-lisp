#pragma once

#include <charStream.h>

#include <string>
#include <utility>
#include <variant>

namespace lexer {

/// @see Token
/// @see tokenTypeName for string representation.
enum class TokenType
{
    start,
    comment,
    keyword,
    literal,
    integer,
    floatpt,
    character,
    string,
    leftParen,
    rightParen,
    whitespace, 
    eof,
    error,
};

/// Converts enum class to string representation.
/// ...because 'enum class'es can't have methods...
std::string tokenTypeName(const TokenType& type);
TokenType charToTokenType(char shorthand);

/// Represents a group of characters with attached meaning and no hierarchical structure.
class Token
{
public: // Members:
    const TokenType mType;
    const std::variant<const std::string, const long, const double, const char> mData;
    const std::string mSrcString;
    const Caret mSrcPos;

public: // Methods:
    Token(TokenType type, std::string chars, Caret pos) : mType(type), mSrcString(std::move(chars)), mSrcPos(pos) {}
    Token(TokenType type, char chars, Caret pos) : mType(type), mSrcString(std::string() + chars), mSrcPos(pos) {}

    // Data access convenience methods:

    /// Equivelent to std::get<std::string>(). Use if `type` is a string.
    [[maybe_unused]] [[nodiscard]] std::string asString() const;
    /// Equivelent to std::get<long>(). Use if `type` is a long.
    [[maybe_unused]] [[nodiscard]] long asLong() const;
    /// Equivelent to std::get<double>(). Use if `type` is a double.
    [[maybe_unused]] [[nodiscard]] double asDouble() const;
    /// Equivelent to std::get<char>(). Use if `type` is a char.
    [[maybe_unused]] [[nodiscard]] char asChar() const;

    // Methods for pretty printing:

    /// Nicely formatted, no trailing new line.
    [[nodiscard]] std::string description() const;
};

} // namespace lexer
