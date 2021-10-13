#pragma once

#include <variant>
#include <iostream>
#include <vector>
#include <memory>

namespace lexer {
class CharStream;
class Token;
}

namespace parser {

// enum class NodeType
// {
//     leaf,
//     list,
// };

class ConsNode;
using ConsList = std::shared_ptr<ConsNode>;
using Atom = std::shared_ptr<lexer::Token>;

/// 
class ConsNode
{
public:
    ConsNode(Atom car = Atom(), ConsList cdr = ConsList()) : mCar(car), mCdr(cdr) {}

    // const NodeType mType;
    // const std::variant<const token::Token*, std::vector<Node*>> mCons;

    // ~Node();

    // std::string first();
    // Node* getAt(const int index) const;
    // std::string getLiteral(const int index) const;
    // std::vector<Node*> getList(const int index) const;
    // void prettyPrint();
    // std::string toFlatStr();

private:
    Atom mCar;
    ConsList mCdr;
};

ConsNode parseTokens(lexer::CharStream& cs);

} // namespace parser
