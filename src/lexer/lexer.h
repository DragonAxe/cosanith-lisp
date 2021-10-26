#pragma once

#include <../charStream/CharStream.h>
#include <token.h>

#include <string>
#include <utility>
#include <variant>
#include <memory>

namespace lexer {

class TokenStream
{
public:
    explicit TokenStream(std::shared_ptr<CharStream> input) : mIn(std::move(input)) {}
    Token get();

private:
    bool mStreamStart = true;
    std::shared_ptr<CharStream> mIn;
};

namespace tests {

[[maybe_unused]] void testScanNumber();
[[maybe_unused]] void testScanInt();
[[maybe_unused]] void testScanFloat();

} // namespace lexer::tests

} // namespace lexer
