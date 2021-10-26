#include <charStream/CharStream.h>
#include <charStream/FileCharStream.h>
#include <charStream/StrCharStream.h>
#include <lexer/lexer.h>
#include <lexer/token.h>
#include <parser/parser.h>
#include <parser/ast.h>

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
bool expectTokens(
    const std::string &note,
    CharStreamT &&in,
    const std::string &expectedSequence,
    bool debug = false)
{
  using namespace std;

  static_assert(std::is_base_of<CharStream, CharStreamT>::value,
      "Input stream parameter must inherit from CharStream.");

  bool success = true;
  lexer::TokenStream ts(make_shared<CharStreamT>(in));

  cout << "#### " << note << " #### ";
  if (debug) { cout << endl; }

  for (const char c: expectedSequence) {
    lexer::Token tok = ts.get();
    lexer::TokenType type = lexer::charToTokenType(c);
    if (tok.mType != type || debug) {
      if (!debug) { cout << "Failed: "; }
      cout << "Expected " << c << "-" << lexer::tokenTypeName(type) << ": "
           << tok.description() << " at "
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

[[maybe_unused]] bool lexTests()
{
  bool success = true;
  // @formatter:off
  success &= expectTokens("identifier", StrCharStream("text"), "0n1");
  success &= expectTokens("1L-comment", StrCharStream("// comment"), "0/1");
  success &= expectTokens("1L-comm /n", StrCharStream("// comment\n"), "0/1");
  success &= expectTokens("ML-comment", StrCharStream("/* **comment** */"), "0/1");
  success &= expectTokens("keyword   ", StrCharStream("fn"), "0k1");
  success &= expectTokens("zero      ", StrCharStream("0"), "0i1");
  success &= expectTokens("integer   ", StrCharStream("5"), "0i1");
  success &= expectTokens("int-neg   ", StrCharStream("-5"), "0i1");
  success &= expectTokens("hex       ", StrCharStream("0xdEAdB33F"), "0i1");
  success &= expectTokens("oct       ", StrCharStream("05723"), "0i1");
  success &= expectTokens("string    ", StrCharStream("\"string\""), "0s1");
  success &= expectTokens("string+W  ", StrCharStream("\"string w\nspace\""), "0s1");
  success &= expectTokens("string+Quo", StrCharStream(R"("<\"\">")"), "0s1");
  success &= expectTokens("floatpt   ", StrCharStream("2.5"), "0f1");
  success &= expectTokens("floatneg  ", StrCharStream("-2.5"), "0f1");
  success &= expectTokens("char      ", StrCharStream("'c'"), "0c1");
  success &= expectTokens("char-escp ", StrCharStream("'\\n'"), "0c1");
  success &= expectTokens("Lparen(   ", StrCharStream("("), "0(1");
  success &= expectTokens("Rparen)   ", StrCharStream(")"), "0)1");
  success &= expectTokens("Whitespace", StrCharStream(" \n "), "0 1");

  success &= expectTokens("Simple parens", StrCharStream("()"), "0()1");
  success &= expectTokens("keyword  ", StrCharStream("(fn ())"), "0(k ())1");
  success &= expectTokens("Integers ", StrCharStream("(2 4 6 8)"), "0(i i i i)1");
  success &= expectTokens("Negatives", StrCharStream("(-2 -4 -6 -8)"), "0(i i i i)1");
  success &= expectTokens("Floats   ", StrCharStream("(2.5 4.8)"), "0(f f)1");
  success &= expectTokens("Function ", StrCharStream("(fn (a) (* 2 a))"), "0(k (n) (n i n))1");
  success &= expectTokens("Subtraction", StrCharStream("(- 2 5)"), "0(n i i)1");
  // expectTokens("File", FileCharStream("../files/lisp.rp"));
  // @formatter:on
  return success;
}


[[maybe_unused]] bool parseTests()
{
  using namespace std;

  lexer::TokenStream tokenStream(make_shared<StrCharStream>(
      "(fn (a b) (+ a 1 b 2))"));
  shared_ptr<parser::SExpr> sExpr = parser::parseTokens(tokenStream);
  cout << sExpr->orderedPairStr() << endl;
  cout << sExpr->listStr() << endl;

  return true;
}


// static std::unique_ptr<llvm::LLVMContext> TheContext;
// static std::unique_ptr<llvm::Module> TheModule;
// static std::unique_ptr<llvm::IRBuilder<>> Builder;
// static std::map<std::string, llvm::Value *> NamedValues;
//
//
// void llvmTest()
// {
//   TheContext = std::make_unique<llvm::LLVMContext>();
//   TheModule = std::make_unique<llvm::Module>("my cool lisp lang", *TheContext);
//   Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
//   std::vector<llvm::Type *> ArgTypes;
//   llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getVoidTy(*TheContext),
//       ArgTypes,
//       false);
//   llvm::Function *TheFunction = llvm::Function::Create(FT,
//       llvm::Function::ExternalLinkage,
//       "MyFun",
//       TheModule.get());
//
//   llvm::BasicBlock *bb = llvm::BasicBlock::Create(*TheContext, "", TheFunction);
//
//   Builder->SetInsertPoint(bb);
//
//   llvm::Constant *retVal = llvm::ConstantInt::get(*TheContext,
//       llvm::APInt(64, 192651));
//   Builder->CreateRet(retVal);
//
//   // Validate the generated code, checking for consistency.
//   verifyFunction(*TheFunction);
//
//   TheModule->print(llvm::errs(), nullptr);
// }


void astTest()
{
  using namespace std;
  // map<std::string, llvm::Value *> llvmNamedValues;

  lexer::TokenStream tokenStream(make_shared<StrCharStream>("(fn double (a) (* 2 a))\n(fn main () (let (x 5) (double x))"));
  shared_ptr<parser::SExpr> sExpr = parser::parseTokens(tokenStream);
  ast::AstModule astModule(sExpr, "testModule");
  shared_ptr<llvm::Module> llvmModule = astModule.codegen();

  llvmModule->print(llvm::errs(), nullptr);
}


int main()
{
  // using namespace std;

  // FileCharStream in("../../files/lisp.rp");

  // llvmTest();
  astTest();

  bool success = true;
  // success &= lexTests();
  // success &= parseTests();

  return success;
}
