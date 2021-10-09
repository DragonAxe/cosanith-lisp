#ifndef lexer_h
#define lexer_h

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

}

#endif // lexer_h