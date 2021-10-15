#include <lexer/lexer.h>
#include <lexer/token.h>
#include <regex/regex.h>
#include <parser/parser.h>

#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <utility>

// class AstBase
// {
// public:
//     virtual void prettyPrint(int indent)
//     {
//         throw std::runtime_error("Attempt to call virtual function AstBase::prettyPrint");
//     }
// };

// /// [0-9]+
// class AstInt : public AstBase
// {
//     void prettyPrint(int indent) override
//     {
//         // using namespace std;

//         // string dt(indent, ' ');
//         // cout << dt << "[AstAdd] operands:" << endl;
//         // for (string& arg : args) {
//         //     cout << arg << " ";
//         // }
//         // cout << ") statements:" << endl;
//         // statement->prettyPrint(indent + 1);
//     }
// };

// /// (+ variable number statements, sums all)
// class AstAdd : public AstBase
// {
// private:
//     std::vector<AstBase*> operands;

// public:
//     explicit AstAdd(const Node* parseTree)
//     {
//         using namespace std;
//         for (int i = 1; i < get<vector<Node*>>(parseTree->item).size(); i++) {
//             Node* param = parseTree->getAt(i);
//             if (param->type == NodeType::leaf) {

//             } else {
//                 string first = param->first();
//                 if (first == "") {

//                 }
//             }
//         }
//     }

//     ~AstAdd()
//     {
//         for (auto& op : operands) {
//             delete op;
//         }
//     }

//     void prettyPrint(int indent) override
//     {
//         using namespace std;

//         string dt(indent, ' ');
//         cout << dt << "[AstAdd] operands:" << endl;
//         for (auto& op : operands) {
//             op->prettyPrint(indent + 2);
//         }
//     }
// };

// /// (call function_name argument list)
// class AstFnCall : public AstBase
// {
// public:
//     explicit AstFnCall(const Node* parseTree)
//     {

//     }

//     void prettyPrint(int indent) override
//     {

//     }
// };

// /// (fn fn_name (arg list) (statement list))
// class AstFnDef : public AstBase
// {
// private:
//     std::string name;
//     std::vector<std::string> args;
//     AstBase* statement;
// public:
//     explicit AstFnDef(const Node* parseTree)
//     {
//         // using namespace std;

//         // name = parseTree->getLiteral(1);
//         // for (Node* arg : parseTree->getList(2)) {
//         //     args.emplace_back(get<token::Token*>(arg->item));
//         // }
//         // Node* statementList = parseTree->getAt(3);

//         // string keyword = statementList->first();
//         // if (keyword == "fn") {
//         //     statement = new AstFnDef(statementList);
//         // } else if (keyword == "call") {
//         //     statement = new AstFnCall(statementList);
//         // } else if (keyword == "+") {
//         //     statement = new AstAdd(statementList);
//         // } else {
//         //     throw runtime_error("Unknown first list element: " + keyword);
//         // }
//     }

//     void prettyPrint(int indent) override
//     {
//         using namespace std;

//         string dt(indent, ' ');
//         cout << dt << "[AstFnDef] name=" << name << " args=( ";
//         for (string& arg : args) {
//             cout << arg << " ";
//         }
//         cout << ") statements:" << endl;
//         statement->prettyPrint(indent + 2);
//     }
// };

// /// Contains a list of functions
// class AstFile : public AstBase
// {
// private:
//     std::vector<AstFnDef*> definitions;

// public:
//     explicit AstFile(const Node* parseTree)
//     {
//         if (parseTree->type == NodeType::list) {
//             auto list = std::get<std::vector<Node*>>(parseTree->item);
//             for (auto& statement : list) {

//                 std::string keyword = statement->first();
//                 if (keyword == "fn") {
//                     definitions.emplace_back(new AstFnDef(statement));
//                 } else {
//                     throw std::runtime_error("Unrecognised statement: " + statement->toFlatStr());
//                 }
//             }
//         } else {
//             throw std::runtime_error("Top level Node must be a list.");
//         }
//     }

//     ~AstFile()
//     {
//         for (auto def : definitions) {
//             delete def;
//         }
//     }

//     void prettyPrint(int indent) override
//     {
//         using namespace std;

//         string dt(indent, ' ');
//         cout << dt << "[AstFile] definitions:" << endl;
//         for (auto& ast : definitions) {
//             ast->prettyPrint(indent + 2);
//         }
//     }
// };

template<class CharStreamT>
bool expectTokens(const std::string& note, CharStreamT &&in, const std::string& expectedSequence, bool debug = false) {
    using namespace std;

    static_assert(std::is_base_of<lexer::CharStream, CharStreamT>::value,
                  "Input stream parameter must inherit from lexer::CharStream.");

    bool success = true;
    lexer::TokenStream ts(make_shared<CharStreamT>(in));

    cout << "#### " << note << " #### ";
    if (debug) { cout << endl; }

    for (const char c: expectedSequence) {
        lexer::Token tok = ts.get();
        lexer::TokenType type = lexer::charToTokenType(c);
        if (tok.mType != type || debug) {
            if (!debug) { cout << "Failed: "; }
            cout << "Expected " << c << "-" << lexer::tokenTypeName(type) << ": " << tok.description() << " at "
                 << in.pos().str() << endl;
        }
        if (tok.mType != type) {
            success = false;
            break;
        }
    }

    if (success) {
        cout << "Success!" << endl;
    }

    return success;
}

bool lexTests() {
    bool success = true;
    success &= expectTokens("identifier", lexer::StrCharStream("text"), "0n1");
    success &= expectTokens("1L-comment", lexer::StrCharStream("// comment"), "0/1");
    success &= expectTokens("1L-comm /n", lexer::StrCharStream("// comment\n"), "0/1");
    success &= expectTokens("ML-comment", lexer::StrCharStream("/* **comment** */"), "0/1");
    success &= expectTokens("keyword   ", lexer::StrCharStream("fn"), "0k1");
    success &= expectTokens("zero      ", lexer::StrCharStream("0"), "0i1");
    success &= expectTokens("integer   ", lexer::StrCharStream("5"), "0i1");
    success &= expectTokens("int-neg   ", lexer::StrCharStream("-5"), "0i1");
    success &= expectTokens("hex       ", lexer::StrCharStream("0xdEAdB33F"), "0i1");
    success &= expectTokens("oct       ", lexer::StrCharStream("05723"), "0i1");
    success &= expectTokens("string    ", lexer::StrCharStream("\"string\""), "0s1");
    success &= expectTokens("string+W  ", lexer::StrCharStream("\"string w\nspace\""), "0s1");
    success &= expectTokens("string+Quo", lexer::StrCharStream(R"("<\"\">")"), "0s1");
    success &= expectTokens("floatpt   ", lexer::StrCharStream("2.5"), "0f1");
    success &= expectTokens("floatneg  ", lexer::StrCharStream("-2.5"), "0f1");
    success &= expectTokens("char      ", lexer::StrCharStream("'c'"), "0c1");
    success &= expectTokens("char-escp ", lexer::StrCharStream("'\\n'"), "0c1");
    success &= expectTokens("Lparen(   ", lexer::StrCharStream("("), "0(1");
    success &= expectTokens("Rparen)   ", lexer::StrCharStream(")"), "0)1");
    success &= expectTokens("Whitespace", lexer::StrCharStream(" \n "), "0 1");

    success &= expectTokens("Simple parens", lexer::StrCharStream("()"), "0()1");
    success &= expectTokens("keyword  ", lexer::StrCharStream("(fn ())"), "0(k ())1");
    success &= expectTokens("Integers ", lexer::StrCharStream("(2 4 6 8)"), "0(i i i i)1");
    success &= expectTokens("Negatives", lexer::StrCharStream("(-2 -4 -6 -8)"), "0(i i i i)1");
    success &= expectTokens("Floats   ", lexer::StrCharStream("(2.5 4.8)"), "0(f f)1");
    success &= expectTokens("Function ", lexer::StrCharStream("(fn (a) (* 2 a))"), "0(k (n) (n i n))1");
    success &= expectTokens("Subtraction", lexer::StrCharStream("(- 2 5)"), "0(n i i)1");
    // expectTokens("File", lexer::FileCharStream("../files/lisp.rp"));
    return success;
}


int main() {
    // using namespace std;

    // lexer::FileCharStream in("/home/dragonaxe/Documents/programming_conan/calculator/lisp.rp");

    bool success = true;
    success &= lexTests();

    // lexer::tests::testScanNumber();
    // lexer::tests::testScanInt();
    // lexer::tests::testScanFloat();

    //     Node* root = parseNodes(in);
    //     // root->prettyPrint();
    //     AstFile ast(root);
    //     ast.prettyPrint(0);
    //     delete root;

    // while (true)
    // {
    //     cout << "token: " << getToken(in).description() << endl;
    //     if (in.eof()) {
    //         break;
    //     }
    // }

    return success;
}
