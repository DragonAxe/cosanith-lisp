#include <charStream.h>
#include <lexer.h>
#include <token.h>

#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <stack>
#include <variant>
#include <vector>
#include <optional>

namespace {

char ctrlCharToChar(const char c)
{
    switch (c) {
    case '\\': return '\\';
    case 'n': return '\n';
    case '"': return '"';
    }
    return c;
}

bool isHexDigit(const char c)
{
    switch (c)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        return true;
    }
    return false;
}

} // anonymous namespace

// ----------------------------------------------------------------------------

namespace lexer {

namespace {

/// Regex:
/// \/\/.*                1.single line comment
/// \/\*(.|\n)*\*\/       2.multi-line comment
Token lexComment(CharStream& in)
{
    using namespace std;

    char c;
    bool wasStar = false;
    stringstream out;
    Caret pos = in.pos();

    out << in.get(); // First forward slash

    switch (in.peek()) {
    case '/':
        while (true) {
            c = in.get();
            out << c;
            if (c == '\n' || in.eof()) { break; }
        }
        break;
    case '*':
        out << in.get();
        while (true) {
            c = in.get();
            out << c;
            if ( (wasStar && c == '/') || in.eof() ) {
                break;
            }
            wasStar = c == '*';
        }
        break;
    }

    return Token(TokenType::comment, out.str(), pos);
}

/// Regex:
/// ".*?"                 3.string
Token lexString(CharStream& in)
{
    using namespace std;

    char c;
    bool wasBackslash = false;
    stringstream out;
    Caret pos = in.pos();

    out << in.get(); // starting quote

    while (true)
    {
        c = in.get();
        out << c;
        if (!wasBackslash && c == '"') {
            break;
        }
        if (in.eof()) {
            throw runtime_error("Unexpected EOF while scanning for string litieral.");
        }
        wasBackslash = c == '\\';
    }
    
    return Token(TokenType::string, out.str(), pos);
}

/// Regex:
/// '\\?.'                11.character literal
Token lexCharacter(CharStream& in)
{
    using namespace std;

    char c;
    bool wasBackslash = false;
    stringstream out;
    Caret pos = in.pos();

    out << in.get(); // first single quote

    c = in.get();
    out << c;
    if (c == '\\') {
        out << in.get(); // get escaped char
    }
    out << in.get(); // ending single quote

    if (in.eof()) {
        throw runtime_error("Unexpected EOF while scanning for charactor litieral.");
    }

    return Token(TokenType::character, out.str(), pos);
}

/// Regex:
/// (                     4.left paren
Token lexLeftParen(CharStream& in)
{
    return Token(TokenType::leftParen, in.get(), in.pos());
}

/// Regex:
/// )                     5.right paren
Token lexRightParen(CharStream& in)
{
    return Token(TokenType::rightParen, in.get(), in.pos());
}

/// Regex:
/// 0                     12.zero
/// 0x[0-9a-fA-F]+        6.hex number
/// 0[0-7]+               7.octal number
Token lexZeroHexOctal(CharStream& in)
{
    using namespace std;

    char c;
    stringstream out;
    Caret pos = in.pos();

    out << in.get(); // prefix zero

    c = in.get();
    if (in.eof() || !(isdigit(c) || c == 'x')) { // Zero
        return Token(TokenType::integer, '0', pos);
    } else if (c == 'x') { // Hex
        out << c; // x
        // There must be at least one digit after the x
        c = in.peek();
        if (!isHexDigit(c) || in.eof()) {
            throw runtime_error("For a hex value, there must be at least one valid hex digit after the '0x'.");
        }
        out << in.get(); // 1st digit
        while (true) {
            c = in.peek();
            if (isHexDigit(c) && !in.eof()) {
                out << in.get();
            } else {
                break;
            }
        }
        return Token(TokenType::integer, out.str(), pos);
    } else { // Octal

    }

    return Token(TokenType::integer, "0xdead", Caret());
}

/// Regex:
/// -?[1-9]*((\.?[0-9]+[df]?)|\.)        8.(+/-) int or float or double
Token lexNumber(CharStream& in)
{
    using namespace std;

    return Token(TokenType::integer, "404", Caret());

    // stringstream out;
    // Caret pos = in.pos();

    // char c = in.get();
    // if (c == '-') {
    //     c = in.get();
    //     if ( !isdigit(c) && !(c == '.') ) {

    //     }
    // }
}

/// Regex:
/// \s+                   10.whitespace
Token lexWhitespace(CharStream& in)
{
    using namespace std;

    std::stringstream out;
    Caret pos = in.pos();
    char c;

    while (true)
    {
        c = in.peek();

        if (in.eof()) {
            goto no_more_whitespace;
        }

        if (isspace(c)) {
            out << (char)in.get();
        } else {
            goto no_more_whitespace;
        }
    }
    no_more_whitespace:;

    return Token(TokenType::whitespace, out.str(), pos);
}

/// Regex:
/// [A-Za-z\+\-\*\/\!\@\#\$\%\^\&\*][A-Za-z0-9\+\-\*\/\!\@\#\$\%\^\&\*]*       9.identifier
Token lexIdentifier(CharStream& in)
{
    using namespace std;

    stringstream out;
    Caret pos = in.pos();

    while ( (isalnum(in.peek()) || (ispunct(in.peek()) && in.peek() != ')')) && !in.eof() ) {
        out << (char)in.get();
    }

    string s = out.str();

    if (s == "fn") {
        return Token(TokenType::keyword, s, pos);
    }

    return Token(TokenType::literal, s, pos);
}

} // anonymous namespace

Token TokenStream::get()
{
    using namespace std;

    // Lexer regular language:
    // \/\/.*|\/\*(.|\n)*\*\/|".*?"|'\\?.'|[()]|0x[0-9a-fA-F]+|0[0-7]0|-?[0-9]*((\.?[0-9]+[df]?)|\.)|[A-Za-z\+\-\*\/\!\@\#\$\%\^\&\*][A-Za-z0-9\+\-\*\/\!\@\#\$\%\^\&\*]*|\s+

    // Possible cases:
    // \/\/.*                1.single line comment
    // \/\*(.|\n)*\*\/       2.multi-line comment
    // ".*?"                 3.string
    // '\\?.'                11.character literal
    // (                     4.left paren
    // )                     5.right paren
    // 0x[0-9a-fA-F]+        6.hex number
    // 0[0-7]*               7.zero or octal number
    // -?[1-9]*((\.?[0-9]+[df]?)|\.)                                              8.(+/-) int or float or double
    // \s+                   10.whitespace
    // [A-Za-z\+\-\*\/\!\@\#\$\%\^\&\*][A-Za-z0-9\+\-\*\/\!\@\#\$\%\^\&\*]*       9.identifier

    // First character to token map (state transition map)
    // /    1|2  single or multi- line comment
    // "    3    string
    // '    11   character literal
    // (    4    left paren
    // )    5    right paren
    // 0    6|7  zero or hex or octal number
    // -    8    negative number
    // 1-9  8    positive number
    // .    8    floatingpoint number
    //      10   whitespace
    // !0-9 9    identifier


    if (mStreamStart) {
        mStreamStart = false;
        return Token(TokenType::start, "start", mIn->pos());
    }    

    char c = mIn->peek();

    if (mIn->eof()) {
        return Token(TokenType::eof, "eof", mIn->pos());
    }

    switch (c)
    {
    case '/':  return lexComment(*mIn);
    case '"':  return lexString(*mIn);
    case '\'': return lexCharacter(*mIn);
    case '(':  return lexLeftParen(*mIn);
    case ')':  return lexRightParen(*mIn);
    case '0':  return lexZeroHexOctal(*mIn);
    case '-':  return lexNumber(*mIn);
    case '.':  return lexNumber(*mIn);
    }

    if (isdigit(c)) {
        return lexNumber(*mIn);
    }

    if (isspace(c)) {
        return lexWhitespace(*mIn);
    }

    return lexIdentifier(*mIn);

    // cout << "Lex error: Unrecognised character: \"" << c << "\"" << endl;
    // throw runtime_error("Lex error: Unrecognised character");
}

} // namespace lexer
