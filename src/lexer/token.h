#ifndef token_h
#define token_h

#include <charStream.h>

#include <string>
#include <variant>

namespace lexer {

/// @see Token
/// @see tokenTypeName for string representation.
enum class TokenType
{
    start,
    keyword,
    literal,
    integer,
    floatpt,
    character,
    string,
    leftParen,
    rightParen,
    whitespace, 
    eof
};

/// Converts enum class to string representation.
/// ...because 'enum class'es can't have methods...
const std::string tokenTypeName(const TokenType& type);
const TokenType charToTokenType(const char shorthand);

/// Represents a group of characters with attached meaning and no hierarchical structure.
class Token
{
public: // Members:
    const TokenType mType;
    const std::variant<const std::string, const long, const double, const char> mData;
    const std::string mSrcString;
    const Caret mSrcPos;

public: // Methods:
    Token(TokenType type, std::string chars, Caret pos) : mType(type), mSrcString(chars), mSrcPos(pos) {}
    Token(TokenType type, char chars, Caret pos) : mType(type), mSrcString(std::string() + chars), mSrcPos(pos) {}

    // Data access convenience methods:

    /// Equivelent to std::get<std::string>(). Use if `type` is a string.
    std::string asString() const;
    /// Equivelent to std::get<long>(). Use if `type` is a long.
    long asLong() const;
    /// Equivelent to std::get<double>(). Use if `type` is a double.
    double asDouble() const;
    /// Equivelent to std::get<char>(). Use if `type` is a char.
    char asChar() const;

    // Methods for pretty printing:

    /// Nicely formatted, no trailing new line.
    std::string description() const;
};

} // namespace lexer

#endif // token_h