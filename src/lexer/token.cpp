#include <token.h>

#include <sstream>
#include <algorithm>

namespace lexer {

std::string tokenTypeName(const TokenType& type)
{

    switch (type)
    {
    case TokenType::start:         return "STRT";
    case TokenType::comment:       return "COMT";
    case TokenType::identifier:    return "IDNT";
    case TokenType::keyword:       return "KEYW";
    case TokenType::integer:       return "INT-";
    case TokenType::string:        return "STR-";
    case TokenType::floatpt:       return "FLOT";
    case TokenType::character:     return "CHAR";
    case TokenType::leftParen:     return "LPAR";
    case TokenType::rightParen:    return "RPAR";
    case TokenType::whitespace:    return "WHIT";
    case TokenType::eof:           return "EOF";
    case TokenType::error:         return "ERR";
    }
}

TokenType charToTokenType(const char shorthand)
{
    switch (shorthand) {
    case '0': return TokenType::start;
    case '/': return TokenType::comment;
    case 'n': return TokenType::identifier;
    case 'k': return TokenType::keyword;
    case 'i': return TokenType::integer;
    case 's': return TokenType::string;
    case 'f': return TokenType::floatpt;
    case 'c': return TokenType::character;
    case '(': return TokenType::leftParen;
    case ')': return TokenType::rightParen;
    case ' ': return TokenType::whitespace;
    case '1': return TokenType::eof;
    default:
        return TokenType::error;
    }
}

std::string Token::description() const
{
    std::stringstream out;
    std::stringstream printableStr;
    for (char c : mSrcString) {
        if (c == '\n') {
            printableStr << "\\n";
        } else {
            printableStr << c;
        }
    }
    out << "[" << tokenTypeName(mType) << ":" << mSrcPos.line() << ":" << mSrcPos.col() << ": `" << printableStr.str() << "`]";
    return out.str();
}

[[maybe_unused]] std::string Token::asString() const
{
    return std::get<const std::string>(mData);
}

[[maybe_unused]] long Token::asLong() const
{
    return std::get<const long>(mData);
}

[[maybe_unused]] double Token::asDouble() const
{
    return std::get<const double>(mData);
}

[[maybe_unused]] char Token::asChar() const
{
    return std::get<const char>(mData);
}

} // namespace lexer
