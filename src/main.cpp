#include <lexer/lexer.h>
#include <lexer/token.h>
#include <parser/parser.h>

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <utility>

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

[[maybe_unused]] bool lexTests() {
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


bool parseTests()
{
    using namespace std;

    lexer::TokenStream tokenStream(make_shared<lexer::StrCharStream>("(fn (a b) (+ a 1 b 2))"));
    shared_ptr<parser::SExpr> sExpr = parser::parseTokens(tokenStream);
    cout << sExpr->orderedPairStr() << endl;
    cout << sExpr->listStr() << endl;

    return true;
}


static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::Module> TheModule;
static std::unique_ptr<llvm::IRBuilder<>> Builder;
static std::map<std::string, llvm::Value *> NamedValues;


void llvmTest()
{
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("my cool lisp lang", *TheContext);
    std::vector<llvm::Type*> ArgTypes;
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getVoidTy(*TheContext), ArgTypes, false);
    llvm::Function* TheFunction = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "MyFun", TheModule.get());

    // llvm::BasicBlock* bb = llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);

    TheModule->print(llvm::errs(), nullptr);
}


int main() {
    // using namespace std;

    // lexer::FileCharStream in("../../files/lisp.rp");

    llvmTest();

    bool success = true;
    // success &= lexTests();
    // success &= parseTests();

    return success;
}
