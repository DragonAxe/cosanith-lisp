#include <charStream.h>
#include <lexer.h>
#include <token.h>

#include <iostream>
#include <istream>
#include <functional>
#include <sstream>

namespace {

[[maybe_unused]] char ctrlCharToChar(const char c)
{
    switch (c) {
    case '\\': return '\\';
    case 'n': return '\n';
    case '"': return '"';
    default:
        return c;
    }
}

[[maybe_unused]] bool isHexDigit(const char c)
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
    default:
        return false;
    }
}

} // anonymous namespace

// ----------------------------------------------------------------------------

namespace lexer {

namespace {

//Token lexIdentifier(CharStream& in, std::string partialStr);

//
// Slurp helpers --------------------------------------------------------------
//

//void getWhile(const std::function<bool (const char)>& condition, CharStream& in, std::stringstream& out)
//{
//    while (true) {
//        char c = in.peek();
//        if (condition(c) && !in.eof()) {
//            out << in.get();
//        } else {
//            break;
//        }
//    }
//}
//
//void getExpect(const std::function<bool (const char)>& condition, CharStream& in, std::stringstream& out)
//{
//    char c = in.peek();
//    bool matches = condition(c);
//    if (matches) {
//        out << in.get();
//    } else {
//        std::stringstream err;
//        err << "Unexpected ";
//        if (in.eof()) {
//            err << "EOF";
//        } else {
//            err << '\'' << c << '\'';
//        }
//        err << " while scanning";
//        throw std::runtime_error(err.str());
//    }
//}

//
// Scanner state machines -----------------------------------------------------
//

///
class ScannerBase
{
public:
    enum class Acceptance {
        accepted,
        rejected,
        undetermined,
    };
    virtual void matchChar(char c) = 0;
    [[nodiscard]] virtual Acceptance acceptance() const = 0;
    [[nodiscard]] virtual TokenType tokenType() const = 0;
};

// ///
// /// 0x[0-9a-fA-F]+        6.hex number
// class ScanHex : ScannerBase
// {
// private:
//     std::function<char (bool)> matcherFn;
// public:
//     explicit ScanHex(std::function<char (bool)> fn) : matcherFn(fn)
//     {}

//     std::optional<lexer::Token> matchChar(char c) override
//     {
//         return std::nullopt;
//     }
// };

// ///
// /// 0[0-7]*               7.zero or octal number
// class ScanOct : ScannerBase
// {
// private:
//     std::vector<ScannerBase*> possibleExpressions;
// public:
//     explicit ScanOct(std::vector<ScannerBase*> possibleExpressions) : possibleExpressions(possibleExpressions)
//     {}

//     std::optional<lexer::Token> matchChar(char c) override
//     {

//         return std::nullopt;
//     }
// };


///
/// \w+                         Whitespace
class ScanWhitespace : public ScannerBase
{
public:
    enum class States {
        start,
        whitespace,
        reject,
    } mState = States::start;

    void matchChar(char c) override
    {
        switch (mState)
        {
        case States::start:
            case States::whitespace:
            if (isspace(c)) { mState = States::whitespace; }
            else { mState = States::reject; }
            break;
        default: break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override
    {
        switch (mState)
        {
        case States::whitespace:
            return Acceptance::accepted;

        case States::start:
            return Acceptance::undetermined;

        default:
            return Acceptance::rejected;
        }
    }
    [[nodiscard]] TokenType tokenType() const override
    {
        return TokenType::whitespace;
    }
};

///
/// -?[0-9]*\.[0-9]+[df]?           8.(+/-) float or double
class ScanFloat : public ScannerBase
{
public:
    enum class States {
        start,
        negative,
        whole,
        point,
        fract,
        suffix,
        reject,
    } mState = States::start;

    void matchChar(char c) override
    {
        switch (mState)
        {
        case States::start:
            if (c == '-') { mState = States::negative; }
            else if (isdigit(c)) { mState = States::whole; }
            else if (c == '.') { mState = States::point; }
            else { mState = States::reject; }
            break;
        case States::negative:
        case States::whole:
            if (isdigit(c)) { mState = States::whole; }
            else if (c == '.') { mState = States::point; }
            else { mState = States::reject; }
            break;
        case States::point:
            if (isdigit(c)) { mState = States::fract; }
            else { mState = States::reject; }
            break;
        case States::fract:
            if (isdigit(c)) { mState = States::fract; }
            else if (c == 'f' || c == 'd') { mState = States::suffix; }
            else { mState = States::reject; }
            break;
        case States::suffix:
            mState = States::reject;
            break;
        default: break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override
    {
        switch (mState)
        {
        case States::fract:
        case States::suffix:
            return Acceptance::accepted;

        case States::start:
        case States::negative:
        case States::whole:
        case States::point:
            return Acceptance::undetermined;

        default:
            return Acceptance::rejected;
        }
    }
    [[nodiscard]] TokenType tokenType() const override
    {
        return TokenType::floatpt;
    }
};

///
/// -?[0-9]+[l]?                    8.(+/-) float or double
class ScanInt : public ScannerBase
{
public:
    enum class States {
        start,
        negative,
        zero,
        digit,
        reject,
    } mState = States::start;

    void matchChar(char c) override
    {
        switch (mState)
        {
        case States::start:
            if (c == '-') { mState = States::negative; }
            else if (isdigit(c) && c != '0') { mState = States::digit; }
            else if (c == '0') { mState = States::zero; }
            else { mState = States::reject; }
            break;
        case States::negative:
            if (isdigit(c) && c != '0') { mState = States::digit; }
            else if (c == '0') { mState = States::zero; }
            else { mState = States::reject; }
            break;
        case States::zero:
            mState = States::reject;
            break;
        case States::digit:
            if (isdigit(c)) { mState = States::digit; }
            else { mState = States::reject; }
            break;
        default: break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override
    {
        switch (mState)
        {
        case States::digit:
        case States::zero:
            return Acceptance::accepted;

        case States::start:
        case States::negative:
            return Acceptance::undetermined;

        default:
            return Acceptance::rejected;
        }
    }
    [[nodiscard]] TokenType tokenType() const override
    {
        return TokenType::integer;
    }
};

///
class ScannerSet
{
private:
    std::vector<std::shared_ptr<ScannerBase>> mScanners;
    std::vector<ScannerBase::Acceptance> mPrevAcceptance;
    bool mAllRejected = false;
public:
    ScannerSet()
    {
        mScanners.emplace_back(std::make_shared<ScanWhitespace>());
        mScanners.emplace_back(std::make_shared<ScanFloat>());
        mScanners.emplace_back(std::make_shared<ScanInt>());
        // Pre-initialize mPrevAcceptance to same size as mScanners
        for (auto& scanner : mScanners) {
            mPrevAcceptance.emplace_back(scanner->acceptance());
        }
    }
    void matchChar(const char c)
    {
        bool allRejected = true;
        for (int i = 0; i < mScanners.size(); i++) {
            mPrevAcceptance[i] = mScanners[i]->acceptance();
            mScanners[i]->matchChar(c);
            if (mScanners[i]->acceptance() != ScannerBase::Acceptance::rejected) {
                allRejected = false;
            }
        }
        mAllRejected = allRejected;
    }
    [[nodiscard]] bool allRejected() const
    {
        return mAllRejected;
    }
    [[nodiscard]] std::shared_ptr<ScannerBase> lastAcceptedScanner() const
    {
        for (int i = 0; i < mScanners.size(); i++) {
            if (mPrevAcceptance[i] == ScannerBase::Acceptance::accepted) {
                return mScanners[i];
            }
        }
        return nullptr;
    }
};


//
// Lexer states ---------------------------------------------------------------
//

///// Regex:
///// \/\/.*                1.single line comment
///// \/\*(.|\n)*\*\/       2.multi-line comment
//Token lexComment(CharStream& in)
//{
//    using namespace std;
//
//    char c;
//    bool wasStar = false;
//    bool wasNewLine = false;
//    stringstream out;
//    Caret pos = in.pos();
//
//    out << in.get(); // First forward slash
//
//    switch (in.peek()) {
//    case '/':
//        out << in.get();
//        getWhile([&wasNewLine](const char c){
//            bool match = wasNewLine;
//            wasNewLine = c == '\n';
//            return !match; }, in, out);
//        // while (true) {
//        //     c = in.get();
//        //     out << c;
//        //     if (c == '\n' || in.eof()) { break; }
//        // }
//        break;
//    case '*':
//        out << in.get();
//        while (true) {
//            c = in.get();
//            out << c;
//            if ( (wasStar && c == '/') || in.eof() ) {
//                break;
//            }
//            wasStar = c == '*';
//        }
//        break;
//    }
//
//    return {TokenType::comment, out.str(), pos};
//}
//
///// Regex:
///// ".*?"                 3.string
//Token lexString(CharStream& in)
//{
//    using namespace std;
//
//    bool wasBackslash = false;
//    stringstream out;
//    Caret pos = in.pos();
//
//    getExpect([](const char c) {
//        return c == '"'; }, in, out);
//
//    getWhile([&wasBackslash](const char c) {
//        bool match = !wasBackslash && c == '"';
//        wasBackslash = c == '\\';
//        return !match; }, in, out);
//
//    getExpect([](const char c) {
//        return c == '"'; }, in, out);
//
//    return {TokenType::string, out.str(), pos};
//}
//
///// Regex:
///// '\\?.'                11.character literal
//Token lexCharacter(CharStream& in)
//{
//    using namespace std;
//
//    char c;
//    bool wasBackslash = false;
//    stringstream out;
//    Caret pos = in.pos();
//
//    out << in.get(); // first single quote
//
//    c = in.get();
//    out << c;
//    if (c == '\\') {
//        out << in.get(); // get escaped char
//    }
//    out << in.get(); // ending single quote
//
//    if (in.eof()) {
//        throw runtime_error("Unexpected EOF while scanning for character literal.");
//    }
//
//    return {TokenType::character, out.str(), pos};
//}
//
///// Regex:
///// (                     4.left paren
//Token lexLeftParen(CharStream& in)
//{
//    return {TokenType::leftParen, in.get(), in.pos()};
//}
//
///// Regex:
///// )                     5.right paren
//Token lexRightParen(CharStream& in)
//{
//    return {TokenType::rightParen, in.get(), in.pos()};
//}
//
///// Regex:
///// 0                     12.zero
///// 0x[0-9a-fA-F]+        6.hex number
///// 0[0-7]+               7.octal number
//Token lexZeroHexOctal(CharStream& in)
//{
//    using namespace std;
//
//    char c;
//    stringstream out;
//    Caret pos = in.pos();
//
//    out << in.get(); // prefix zero
//
//    c = in.get();
//    if (in.eof() || !(isdigit(c) || c == 'x')) { // Zero
//        return {TokenType::integer, '0', pos};
//    } else if (c == 'x') { // Hex
//        out << c; // x
//        // There must be at least one digit after the x
//        c = in.peek();
//        if (!isHexDigit(c) || in.eof()) {
//            throw runtime_error("For a hex value, there must be at least one valid hex digit after the '0x'.");
//        }
//        out << in.get(); // 1st digit
//        while (true) {
//            c = in.peek();
//            if (isHexDigit(c) && !in.eof()) {
//                out << in.get();
//            } else {
//                break;
//            }
//        }
//        return {TokenType::integer, out.str(), pos};
//    } else { // Octal
//        throw runtime_error("Octal not implemented.");
//    }
//}
//
///// Regex:
///// -?([1-9][0-9]*)?((\.?[0-9]+[df]?)|\.)     8.(+/-) int or float or double
//Token lexNumber(CharStream& in)
//{
//    using namespace std;
//
//    char c;
//    stringstream out;
//
//    c = in.peek();
//    if (c == '-') {
//        out << in.get(); // negative
//        c = in.peek();
//        if (!(isdigit(c) || c == '.')) {
//            lexIdentifier(in, out.str());
//        }
//    }
//    if (c == 0) {
//        throw runtime_error("Number cannot start with zero");
//    }
//
//
//
//
//    return {TokenType::integer, "404", Caret()};
//
//    // stringstream out;
//    // Caret pos = in.pos();
//
//    // char c = in.get();
//    // if (c == '-') {
//    //     c = in.get();
//    //     if ( !isdigit(c) && !(c == '.') ) {
//
//    //     }
//    // }
//}
//
///// Regex:
///// \s+                   10.whitespace
//Token lexWhitespace(CharStream& in)
//{
//    using namespace std;
//
//    std::stringstream out;
//    Caret pos = in.pos();
//    char c;
//
//    while (true)
//    {
//        c = in.peek();
//
//        if (in.eof()) {
//            goto no_more_whitespace;
//        }
//
//        if (isspace(c)) {
//            out << (char)in.get();
//        } else {
//            goto no_more_whitespace;
//        }
//    }
//    no_more_whitespace:;
//
//    return {TokenType::whitespace, out.str(), pos};
//}
//
///// Regex:
///// [A-Za-z\+\-\*\/\!\@\#\$\%\^\&\*][A-Za-z0-9\+\-\*\/\!\@\#\$\%\^\&\*]*       9.identifier
//Token lexIdentifier(CharStream& in)
//{
//    using namespace std;
//
//    stringstream out;
//    Caret pos = in.pos();
//
//    while ( (isalnum(in.peek()) || (ispunct(in.peek()) && in.peek() != ')')) && !in.eof() ) {
//        out << (char)in.get();
//    }
//
//    string s = out.str();
//
//    if (s == "fn") {
//        return {TokenType::keyword, s, pos};
//    }
//
//    return {TokenType::literal, s, pos};
//}

} // anonymous namespace

Token TokenStream::get()
{
    using namespace std;

    if (mStreamStart) {
        mStreamStart = false;
        return {TokenType::start, "start", mIn->pos()};
    }

    Caret pos = mIn->pos();
    char c;

    if (mIn->eof()) {
        return {TokenType::eof, "eof", pos};
    }

    std::stringstream out;
    ScannerSet scannerSet;

    while (true) {
        c = mIn->peek();

        scannerSet.matchChar(c);
        if (scannerSet.allRejected()) {
            break;
        }

        out << mIn->get();
    }

    shared_ptr<ScannerBase> acceptedScanner = scannerSet.lastAcceptedScanner();

    return {acceptedScanner->tokenType(), out.str(), pos};

    // throw runtime_error("WIP");

    // Lexer regular language:
    // \/\/.*|\/\*(.|\n)*\*\/|".*?"|'\\?.'|[()]|0x[0-9a-fA-F]+|0[0-7]0|-?([1-9][0-9]*)?((\.?[0-9]+[df]?)|\.)|[A-Za-z\+\-\*\/\!\@\#\$\%\^\&\*][A-Za-z0-9\+\-\*\/\!\@\#\$\%\^\&\*]*|\s+

    // Possible cases:
    // \/\/.*                1.single line comment
    // \/\*(.|\n)*\*\/       2.multi-line comment
    // ".*?"                 3.string
    // '\\?.'                11.character literal
    // (                     4.left paren
    // )                     5.right paren
    // 0                     12. zero
    // 0x[0-9a-fA-F]+        6.hex number
    // 0[0-7]*               7.zero or octal number
    // -?([1-9][0-9]*)?((\.?[0-9]+[df]?)|\.)                                      8.(+/-) int or float or double
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
    // .    8    floatingPoint number
    //      10   whitespace
    // !0-9 9    identifier


    // if (mStreamStart) {
    //     mStreamStart = false;
    //     return Token(TokenType::start, "start", mIn->pos());
    // }

    // char c = mIn->peek();

    // if (mIn->eof()) {
    //     return Token(TokenType::eof, "eof", mIn->pos());
    // }

    // switch (c)
    // {
    // case '/':  return lexComment(*mIn);
    // case '"':  return lexString(*mIn);
    // case '\'': return lexCharacter(*mIn);
    // case '(':  return lexLeftParen(*mIn);
    // case ')':  return lexRightParen(*mIn);
    // case '0':  return lexZeroHexOctal(*mIn);
    // case '-':  return lexNumber(*mIn);
    // case '.':  return lexNumber(*mIn);
    // }

    // if (isdigit(c)) { // [1-9], zero is captured earlier
    //     return lexNumber(*mIn);
    // }

    // if (isspace(c)) {
    //     return lexWhitespace(*mIn);
    // }

    // return lexIdentifier(*mIn);

    // cout << "Lex error: Unrecognised character: \"" << c << "\"" << endl;
    // throw runtime_error("Lex error: Unrecognised character");
}

namespace tests { // lexer::tests

namespace {

void testHelper(ScannerBase&& scanner, ScannerBase::Acceptance expectedResult, const std::string& testString)
{
    for (const char c : testString) {
        scanner.matchChar(c);
    }
    if (scanner.acceptance() == expectedResult) {
        std::cout << "Success!";
    } else {
        std::cout << "Failure!";
    }
    std::cout << " String='" << testString << "'" << std::endl;
}

} // lexer::tests::anonymous namespace

[[maybe_unused]] void testScanNumber()
{
    ScanInt i;
    ScanFloat f;
    std::vector<ScannerBase*> scanners{&i, &f};
    std::stringstream out;

    std::string testStr = "-239842341.234589283 other";
    for (const char c : testStr) {
        for (ScannerBase* scanner : scanners) {
            scanner->matchChar(c);
        }

        bool allReject = true;
        for (const ScannerBase* scanner : scanners) {
            if (scanner->acceptance() != ScannerBase::Acceptance::rejected) {
                allReject = false;
            }
        }
        if (allReject) {
            break;
        }

        out << c;
    }

    std::cout << "Lexed: '" << out.str() << "'" << std::endl;
}

[[maybe_unused]] void testScanInt()
{
    using acc = ScannerBase::Acceptance;
    testHelper(ScanInt(), acc::accepted, "0");
    testHelper(ScanInt(), acc::accepted, "-0");
    testHelper(ScanInt(), acc::accepted, "5");
    testHelper(ScanInt(), acc::accepted, "-5");
    testHelper(ScanInt(), acc::accepted, "50");
    testHelper(ScanInt(), acc::accepted, "-50");

    testHelper(ScanInt(), acc::rejected, "-");
    testHelper(ScanInt(), acc::rejected, "05");
    testHelper(ScanInt(), acc::rejected, "-05");

    testHelper(ScanInt(), acc::rejected, "num");
}

[[maybe_unused]] void testScanFloat()
{
    using acc = ScannerBase::Acceptance;
    testHelper(ScanFloat(), acc::accepted, "0.0");
    testHelper(ScanFloat(), acc::accepted, "-0.0");
    testHelper(ScanFloat(), acc::accepted, ".0");
    testHelper(ScanFloat(), acc::accepted, "-.0");
    testHelper(ScanFloat(), acc::accepted, "50.10");
    testHelper(ScanFloat(), acc::accepted, "-50.10");
    testHelper(ScanFloat(), acc::accepted, "05.10");
    testHelper(ScanFloat(), acc::accepted, "-05.10");
    testHelper(ScanFloat(), acc::accepted, "-05.10f");
    testHelper(ScanFloat(), acc::accepted, "-05.10d");

    testHelper(ScanFloat(), acc::undetermined, "-0.");
    testHelper(ScanFloat(), acc::undetermined, "0.");
    testHelper(ScanFloat(), acc::undetermined, ".");

    // Possible future work
    testHelper(ScanFloat(), acc::rejected, "nan");
    testHelper(ScanFloat(), acc::rejected, "-nan");
}

} // namespace lexer::tests

} // namespace lexer
