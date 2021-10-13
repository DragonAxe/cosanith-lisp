#pragma once

#include <charStream.h>

#include <string>
#include <variant>
#include <memory>

namespace lexer {
class Token;

class TokenStream
{
public:
    explicit TokenStream(std::shared_ptr<CharStream> input) : mIn(input) {}
    Token get();

private:
    bool mStreamStart = true;
    std::shared_ptr<CharStream> mIn;
};

namespace tests {

void testScanNumber();
void testScanInt();
void testScanFloat();

} // namespace lexer::tests

} // namespace lexer
