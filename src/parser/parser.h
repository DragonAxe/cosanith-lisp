#pragma once

#include <../lexer/lexer.h>

#include <utility>
#include <variant>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>

namespace lexer {
class TokenStream;
class Token;
}

namespace parser {

class Atom {
public:
    enum class Type {
        sExpr,
        token,
    };
    virtual Type type() = 0;
    virtual std::string orderedPairStr() = 0;
    virtual std::string listStr() = 0;
};

class SExpr {
public:
    std::shared_ptr<Atom> car{nullptr};
    std::shared_ptr<SExpr> cdr{nullptr};

    std::string orderedPairStr()
    {
        using namespace std;
        stringstream out;
        if (car || cdr) {
            out << "(";
            if (car) {
                out << car->orderedPairStr();
            } else {
                out << "nil1";
            }
            out << " . ";
            if (cdr) {
                out << cdr->orderedPairStr();
            } else {
                out << "nil2";
            }
            out << ")";
        } else {
            out << "nil";
        }
        return out.str();
    }

    std::string listStr()
    {
        using namespace std;

        stringstream out;

        SExpr* current = this;

        out << "(";
        while (current->cdr) {
            if (current->car) {
                out << current->car->listStr() << " ";
            } else {
                out << "nil";
            }

            current = current->cdr.get();
        }
        if (current->car) {
            out << current->car->listStr();
        } else {
            out << "nil";
        }
        out << ")";

        return out.str();
    }
};

class AtomSExpr : public Atom {
public:
    std::shared_ptr<SExpr> mExpr;

    AtomSExpr() = default;
    explicit AtomSExpr(std::shared_ptr<SExpr> sExpr) : mExpr(std::move(sExpr)) {}

    Type type() override { return Atom::Type::sExpr; }

    std::string orderedPairStr() override {
        if (mExpr) {
            return mExpr->orderedPairStr();
        } else {
            return "null";
        }
    }

    std::string listStr() override {
        if (mExpr) {
            return mExpr->listStr();
        } else {
            return "null";
        }
    }
};

class AtomToken : public Atom {
public:
    std::shared_ptr<lexer::Token> mToken;

    explicit AtomToken(std::shared_ptr<lexer::Token> token) : mToken(std::move(token)) {}

//    explicit AtomToken(lexer::Token token) : t(token) {}

    Type type() override { return Atom::Type::token; }

    std::string orderedPairStr() override {
        if (mToken) {
            return mToken->mSrcString;
        } else {
            return "null";
        }
    }

    std::string listStr() override {
        if (mToken) {
            return mToken->mSrcString;
        } else {
            return "null";
        }
    }
};

std::shared_ptr<SExpr> parseTokens(lexer::TokenStream& in);

} // namespace parser
