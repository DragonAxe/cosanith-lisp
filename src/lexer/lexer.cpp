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

} // anonymous namespace

// ----------------------------------------------------------------------------

namespace lexer {

namespace {

Token lexLiteralOrKeyword(CharStream& in)
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

long lexInteger(std::istream& in)
{
    return -1;
}

double lexFloatpt(std::istream& in)
{
    return -1.1;
}

char lexCharacter(std::istream& in)
{
    return '~';
}

Token lexLeftParen(CharStream& in)
{
    return Token(TokenType::leftParen, in.get(), in.pos());
}

Token lexRightParen(CharStream& in)
{
    return Token(TokenType::rightParen, in.get(), in.pos());
}

Token lexString(CharStream& in)
{
    using namespace std;

    stringstream out;
    Caret pos = in.pos();

    in.get(); // Discard first quote '"'

    while (true)
    {
        char c = in.peek();
        if (c == '"') {
            break;
        }
        if (c =='\\') {
            in.get(); // discard ctrl backslash
            out << ctrlCharToChar(in.get());
            continue;
        }
        out << in.get();
    }
    
    return Token(TokenType::string, out.str(), pos);
}

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

} // anonymous namespace

Token TokenStream::get()
{
    using namespace std;

    if (mStreamStart) {
        mStreamStart = false;
        return Token(TokenType::start, "start", mIn->pos());
    }    

    char c = mIn->peek();

    if (mIn->eof()) {
        return Token(TokenType::eof, "", mIn->pos());
    }

    switch (c)
    {
    case '(':
        return lexLeftParen(*mIn);
    case ')':
        return lexRightParen(*mIn);
    case '"':
        return lexString(*mIn);
    case '-':
        // number or symbolic identifier
        break;
    }

    if (isspace(c)) {
        return lexWhitespace(*mIn);
    } else if ( isalnum(c) || ispunct(c) ) {
        return lexLiteralOrKeyword(*mIn);
    } else {
        
    }

    cout << "Lex error: Unrecognised character: \"" << c << "\"" << endl;
    throw runtime_error("Lex error: Unrecognised character");
}

} // namespace lexer