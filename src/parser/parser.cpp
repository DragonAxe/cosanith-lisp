#include <parser.h>
#include <../lexer/lexer.h>

#include <istream>
#include <fstream>
#include <string>
#include <sstream>
#include <stack>
#include <optional>

namespace parser {

// Node::~Node()
// {
//     if (type == NodeType::list) {
//         auto list = std::get<std::vector<Node*>>(item);
//         for (auto& node : list) {
//             delete node;
//         }
//     }
// }

// void Node::prettyPrint()
// {
//     if (type == NodeType::leaf) {
//         std::cout << std::get<const token::Token*>(item);
//     } else {
//         std::cout << "( ";
//         auto list = std::get<std::vector<Node*>>(item);
//         for (auto& node : list) {
//             node->prettyPrint();
//             std::cout << " ";
//         }
//         std::cout << ")";
//     }
// }

// std::string Node::toFlatStr()
// {
//     using namespace std;

//     stringstream out;

//     if (type == NodeType::leaf) {
//         out << ":" << get<const token::Token*>(item)->dataToString() << ":";
//     } else {
//         out << "( ";
//         auto list = get<vector<Node*>>(item);
//         for (Node* node : list) {
//             if (node->type == NodeType::leaf) {
//                 out << get<const token::Token*>(node->item)->dataToString();
//             } else {
//                 out << "(..)";
//             }
//             out << " ";
//         }
//         out << ")";
//     }

//     return out.str();
// }

// Node* Node::getAt(const int index) const
// {
//     using namespace std;

//     if (type == NodeType::leaf) {
//         throw runtime_error("Node is not a list. It is a litteral: '" + get<const token::Token*>(item)->dataToString() + "'");
//     }

//     auto list = get<std::vector<Node*>>(item);
//     if (list.size() < 1) {
//         throw runtime_error("List index out of bounds: index=" + to_string(index) + " size=" + to_string(list.size()));
//     }

//     return list[index];
// }

// std::string Node::first()
// {
//     return getLiteral(0);
// }

// std::string Node::getLiteral(const int index) const
// {
//     using namespace std;
//     Node* element = getAt(index);

//     if (element->type != NodeType::leaf) {
//         throw std::runtime_error("Node at index " + to_string(index) + " is not a identifier: " + element->toFlatStr());
//     }

//     return get<const token::Token*>(element->item)->dataToString();
// }

// std::vector<Node*> Node::getList(const int index) const
// {
//     using namespace std;
//     Node* element = getAt(index);

//     if (element->type != NodeType::list) {
//         throw std::runtime_error("Node at index " + to_string(index) + " is not a list: " + get<const token::Token*>(element->item)->dataToString());
//     }

//     return get<vector<Node*>>(element->item);

// }

ConsNode parseTokens(lexer::CharStream& cs)
{
    using namespace std;

    // stack<Node*> nodes;
    ConsNode root;
    // nodes.push(root); // root node
    
    // while (true) {
    //     const token::Token tok = lexer::getToken(in);
    //     if (tok.type == token::TokenType::eof)
    //     {
    //         break;
    //     }

    //     switch (tok.type)
    //     {
    //     case token::TokenType::identifier:
    //         get<vector<Node*>>(nodes.top()->item).emplace_back(new Node{NodeType::leaf, tok});
    //         break;
    //     case token::TokenType::integer:
    //         break;
    //     case token::TokenType::floatpt:
    //         break;
    //     case token::TokenType::character:
    //         break;
    //     case token::TokenType::start:
    //         break;
    //     case token::TokenType::leftParen:
    //         {
    //             Node* sub_node = new Node{NodeType::list, vector<Node*>{}};
    //             get<vector<Node*>>(nodes.top()->item).emplace_back(sub_node);
    //             nodes.push(sub_node);
    //         }
    //         break;
    //     case token::TokenType::eof:
    //         break;
    //     case token::TokenType::rightParen:
    //         nodes.pop();
    //         break;
    //     case token::TokenType::string:
    //         break;
    //     case token::TokenType::whitespace:
    //         break;
    //     }
    // }

    return root;
}

} // namespace parser