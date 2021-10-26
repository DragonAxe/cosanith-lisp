#include <parser.h>

#include <istream>
#include <string>
#include <optional>

namespace parser {

/// S => S E
/// E => ( E I )
/// I => Term | epsilon
std::shared_ptr<SExpr> parseTokens(lexer::TokenStream& in)
{
    using namespace std;

    auto root = make_shared<SExpr>();

    start_over:;
    shared_ptr<lexer::Token> tok = make_shared<lexer::Token>(in.get());

    switch (tok->mType) {
        case lexer::TokenType::start:
        case lexer::TokenType::comment:
        case lexer::TokenType::whitespace:
            goto start_over;
        case lexer::TokenType::keyword:
        case lexer::TokenType::identifier:
        case lexer::TokenType::integer:
        case lexer::TokenType::floatpt:
        case lexer::TokenType::character:
        case lexer::TokenType::string:
            root->car = make_shared<AtomToken>(tok);
            root->cdr = parseTokens(in);
            break;
        case lexer::TokenType::leftParen:
            root->car = make_shared<AtomSExpr>(parseTokens(in));
            root->cdr = parseTokens(in);
            break;
        case lexer::TokenType::rightParen:
        case lexer::TokenType::eof:
            break;
        case lexer::TokenType::error:
            throw runtime_error("Error token encountered: " + tok->description());
    }

    if (root->car || root->cdr) {
        return root;
    } else {
        return nullptr;
    }

}

std::optional<std::shared_ptr<Atom>> SExpr::operator[](std::size_t index) const
{
  auto* currentExpr = this;
  int i = 0;
  for (; i < index; i++) {
    if (!currentExpr->cdr) {
      break;
    }
    currentExpr = currentExpr->cdr.get();
  }
  if (i == index) {
    return std::make_optional(currentExpr->car);
  }
  return std::nullopt;
}

} // namespace parser
