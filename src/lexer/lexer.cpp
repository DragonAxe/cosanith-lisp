#include <charStream.h>
#include <lexer.h>
#include <token.h>

#include <iostream>
#include <istream>
#include <functional>
#include <sstream>

namespace {

[[maybe_unused]] char ctrlCharToChar(const char c) {
    switch (c) {
        case '\\':
            return '\\';
        case 'n':
            return '\n';
        case '"':
            return '"';
        default:
            return c;
    }
}

[[maybe_unused]] bool isHexDigit(const char c) {
    switch (c) {
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
class ScannerBase {
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

/// //                       One Line Comment
/// /* */                    Multi-line Comment
class ScanComment : public ScannerBase {
public:
    enum class States {
        start,
        firstSlash, secondSlash, newline,
        firstStar, secondStar, endSlash,
        reject,
    } mState = States::start;

    void matchChar(char c) override {
        switch (mState) {
            case States::start:
                if (c == '/') { mState = States::firstSlash; }
                else { mState = States::reject; }
                break;
            case States::firstSlash:
                if (c == '/') { mState = States::secondSlash; }
                else if (c == '*') { mState = States::firstStar; }
                else { mState = States::reject; }
                break;
            case States::secondSlash:
                if (c == '\n') { mState = States::newline; }
                break;
            case States::newline:
                mState = States::reject;
                break;
            case States::firstStar:
                if (c == '*') { mState = States::secondStar; }
                break;
            case States::secondStar:
                if (c == '*') { mState = States::secondStar; }
                if (c == '/') { mState = States::endSlash; }
                else { mState = States::firstStar; }
                break;
            case States::endSlash:
                mState = States::reject;
                break;
            default:
                break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override {
        switch (mState) {
            case States::newline:
            case States::endSlash:
            case States::secondSlash:
            case States::firstStar:
            case States::secondStar:
                return Acceptance::accepted;
            case States::start:
            case States::firstSlash:
                return Acceptance::undetermined;
            default:
                return Acceptance::rejected;
        }
    }

    [[nodiscard]] TokenType tokenType() const override {
        return TokenType::comment;
    }
};

/// ".*"                       String literal
class ScanString : public ScannerBase {
public:
    enum class States {
        start,
        openQuote,
        content,
        escapeSlash,
        closeQuote,
        reject,
    } mState = States::start;

    void matchChar(char c) override {
        switch (mState) {
            case States::start:
                if (c == '"') { mState = States::openQuote; }
                else { mState = States::reject; }
                break;
            case States::openQuote:
                if (c != '\\') { mState = States::content; }
                else { mState = States::escapeSlash; }
                break;
            case States::content:
                if (c == '"') { mState = States::closeQuote; }
                else if (c == '\\') { mState = States::escapeSlash; }
                break;
            case States::escapeSlash:
                mState = States::content;
                break;
            case States::closeQuote:
                mState = States::reject;
                break;
            default:
                break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override {
        switch (mState) {
            case States::closeQuote:
                return Acceptance::accepted;
            case States::start:
            case States::openQuote:
            case States::content:
            case States::escapeSlash:
                return Acceptance::undetermined;
            default:
                return Acceptance::rejected;
        }
    }

    [[nodiscard]] TokenType tokenType() const override {
        return TokenType::string;
    }
};

/// ".*"                       String literal
class ScanCharLiteral : public ScannerBase {
public:
    enum class States {
        start,
        openQuote,
        content,
        escapeSlash,
        closeQuote,
        reject,
    } mState = States::start;

    void matchChar(char c) override {
        switch (mState) {
            case States::start:
                if (c == '\'') { mState = States::openQuote; }
                else { mState = States::reject; }
                break;
            case States::openQuote:
                if (c != '\\') { mState = States::content; }
                else { mState = States::escapeSlash; }
                break;
            case States::content:
                if (c == '\'') { mState = States::closeQuote; }
                else { mState = States::reject; }
                break;
            case States::escapeSlash:
                mState = States::content;
                break;
            case States::closeQuote:
                mState = States::reject;
                break;
            default:
                break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override {
        switch (mState) {
            case States::closeQuote:
                return Acceptance::accepted;
            case States::start:
            case States::openQuote:
            case States::content:
            case States::escapeSlash:
                return Acceptance::undetermined;
            default:
                return Acceptance::rejected;
        }
    }

    [[nodiscard]] TokenType tokenType() const override {
        return TokenType::character;
    }
};

///
/// (      Left parenthesis
/// )      Right parenthesis
class ScanParen : public ScannerBase {
public:
    enum class States {
        start,
        lParen,
        rParen,
        reject,
    } mState = States::start;
    bool mIsLeft = false;

    void matchChar(char c) override {
        switch (mState) {
            case States::start:
                if (c == '(') {
                    mState = States::lParen;
                    mIsLeft = true;
                } else if (c == ')') {
                    mState = States::rParen;
                    mIsLeft = false;
                } else { mState = States::reject; }
                break;
            case States::lParen:
            case States::rParen:
                mState = States::reject;
                break;
            default:
                break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override {
        switch (mState) {
            case States::lParen:
            case States::rParen:
                return Acceptance::accepted;

            case States::start:
                return Acceptance::undetermined;

            default:
                return Acceptance::rejected;
        }
    }

    [[nodiscard]] TokenType tokenType() const override {
        if (mIsLeft) {
            return TokenType::leftParen;
        } else {
            return TokenType::rightParen;
        }
    }
};

///
/// \w+                         Whitespace
class ScanWhitespace : public ScannerBase {
public:
    enum class States {
        start,
        whitespace,
        reject,
    } mState = States::start;

    void matchChar(char c) override {
        switch (mState) {
            case States::start:
            case States::whitespace:
                if (isspace(c)) { mState = States::whitespace; }
                else { mState = States::reject; }
                break;
            default:
                break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override {
        switch (mState) {
            case States::whitespace:
                return Acceptance::accepted;

            case States::start:
                return Acceptance::undetermined;

            default:
                return Acceptance::rejected;
        }
    }

    [[nodiscard]] TokenType tokenType() const override {
        return TokenType::whitespace;
    }
};

///
/// -?[0-9]*\.[0-9]+[df]?           8.(+/-) float or double
class ScanFloat : public ScannerBase {
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

    void matchChar(char c) override {
        switch (mState) {
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
            default:
                break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override {
        switch (mState) {
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

    [[nodiscard]] TokenType tokenType() const override {
        return TokenType::floatpt;
    }
};

///
/// -?[0-9]+[l]?                    8.(+/-) whole number
class ScanInt : public ScannerBase {
public:
    enum class States {
        start,
        negative,
        zero,
        digit,
        reject,
    } mState = States::start;

    void matchChar(char c) override {
        switch (mState) {
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
            default:
                break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override {
        switch (mState) {
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

    [[nodiscard]] TokenType tokenType() const override {
        return TokenType::integer;
    }
};

///
/// 0x[0-9a-fA-F]+        6.hex number
class ScanHex : public ScannerBase {
public:
    enum class States {
        start,
        zero,
        x,
        hexDigit,
        reject,
    } mState = States::start;

    void matchChar(char c) override {
        switch (mState) {
            case States::start:
                if (c == '0') { mState = States::zero; }
                else { mState = States::reject; }
                break;
            case States::zero:
                if (c == 'x') { mState = States::x; }
                else { mState = States::reject; }
                break;
            case States::x:
                if (isHexDigit(c)) { mState = States::hexDigit; }
                else { mState = States::reject; }
                break;
            case States::hexDigit:
                if (!isHexDigit(c)) { mState = States::reject; }
                break;
            default:
                break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override {
        switch (mState) {
            case States::hexDigit:
                return Acceptance::accepted;
            case States::start:
            case States::zero:
            case States::x:
                return Acceptance::undetermined;
            default:
                return Acceptance::rejected;
        }
    }

    [[nodiscard]] TokenType tokenType() const override {
        return TokenType::integer;
    }
};

///
/// \w+                         Whitespace
class ScanIdentifier : public ScannerBase {
public:
    enum class States {
        start,
        firstChar,
        reject,
    } mState = States::start;

    void matchChar(char c) override {
        switch (mState) {
            case States::start:
                if (isspace(c)) { mState = States::reject; }
                else if (isdigit(c)) { mState = States::reject; }
                else if (c == '(') { mState = States::reject; }
                else if (c == ')') { mState = States::reject; }
                else { mState = States::firstChar; }
                break;
            case States::firstChar:
                if (isspace(c)) { mState = States::reject; }
                else if (c == '(') { mState = States::reject; }
                else if (c == ')') { mState = States::reject; }
                break;
            default:
                break;
        }
    }

    [[nodiscard]] Acceptance acceptance() const override {
        switch (mState) {
            case States::firstChar:
                return Acceptance::accepted;
            case States::start:
                return Acceptance::undetermined;
            default:
                return Acceptance::rejected;
        }
    }

    [[nodiscard]] TokenType tokenType() const override {
        return TokenType::identifier;
    }

    [[nodiscard]] static bool isKeyword(const std::string& identifier) {
        return identifier == "fn";
    }
};

///
class ScannerSet {
private:
    std::vector<std::shared_ptr<ScannerBase>> mScanners;
    std::vector<ScannerBase::Acceptance> mPrevAcceptance;
    bool mAllRejected = false;
public:
    ScannerSet() {
        mScanners.emplace_back(std::make_shared<ScanComment>());
        mScanners.emplace_back(std::make_shared<ScanParen>());
        mScanners.emplace_back(std::make_shared<ScanInt>());
        mScanners.emplace_back(std::make_shared<ScanHex>());
        mScanners.emplace_back(std::make_shared<ScanFloat>());
        mScanners.emplace_back(std::make_shared<ScanString>());
        mScanners.emplace_back(std::make_shared<ScanCharLiteral>());
        mScanners.emplace_back(std::make_shared<ScanWhitespace>());
        mScanners.emplace_back(std::make_shared<ScanIdentifier>());
        // Pre-initialize mPrevAcceptance to same size as mScanners
        for (auto &scanner: mScanners) {
            mPrevAcceptance.emplace_back(scanner->acceptance());
        }
    }

    void matchChar(const char c) {
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

    [[nodiscard]] bool allRejected() const {
        return mAllRejected;
    }

    [[nodiscard]] std::shared_ptr<ScannerBase> lastAcceptedScanner() const {
        for (int i = 0; i < mScanners.size(); i++) {
            if (mPrevAcceptance[i] == ScannerBase::Acceptance::accepted) {
                return mScanners[i];
            }
        }
        return nullptr;
    }
};

} // anonymous namespace

Token TokenStream::get() {
    // Lexer regular language:
    // \/\/.*|\/\*(.|\n)*\*\/|".*?"|'\\?.'|[()]|0x[0-9a-fA-F]+|0[0-7]0|-?([1-9][0-9]*)?((\.?[0-9]+[df]?)|\.)|[A-Za-z\+\-\*\/\!\@\#\$\%\^\&\*][A-Za-z0-9\+\-\*\/\!\@\#\$\%\^\&\*]*|\s+

    // Possible cases:
    // \/\/.*                1.single line comment
    // \/\*(.|\n)*\*\/       2.multi-line comment
    // ".*?"                 3.string
    // '\\?.'                11.character identifier
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
    // '    11   character identifier
    // (    4    left paren
    // )    5    right paren
    // 0    6|7  zero or hex or octal number
    // -    8    negative number
    // 1-9  8    positive number
    // .    8    floatingPoint number
    //      10   whitespace
    // !0-9 9    identifier

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
        if (scannerSet.allRejected() || mIn->eof()) {
            break;
        }

        out << mIn->get();
    }

    shared_ptr<ScannerBase> acceptedScanner = scannerSet.lastAcceptedScanner();

    if (acceptedScanner) {
        string tokenString = out.str();
        if (acceptedScanner->tokenType() == TokenType::identifier && ScanIdentifier::isKeyword(tokenString)) {
            return {TokenType::keyword, tokenString, pos};
        }
        return {acceptedScanner->tokenType(), out.str(), pos};
    } else {
        return {TokenType::error, std::string(1, c), pos};
    }
}

namespace tests { // lexer::tests

namespace {

void
testHelper(ScannerBase &&scanner, ScannerBase::Acceptance expectedResult, const std::string &testString) {
    for (const char c: testString) {
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

[[maybe_unused]] void testScanNumber() {
    ScanInt i;
    ScanFloat f;
    std::vector<ScannerBase *> scanners{&i, &f};
    std::stringstream out;

    std::string testStr = "-239842341.234589283 other";
    for (const char c: testStr) {
        for (ScannerBase *scanner: scanners) {
            scanner->matchChar(c);
        }

        bool allReject = true;
        for (const ScannerBase *scanner: scanners) {
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

[[maybe_unused]] void testScanInt() {
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

[[maybe_unused]] void testScanFloat() {
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
