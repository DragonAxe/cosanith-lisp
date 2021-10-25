#include <ast.h>

#include <parser.h>

static std::shared_ptr<llvm::LLVMContext> TheContext;
static std::shared_ptr<llvm::Module> TheModule;
static std::shared_ptr<llvm::IRBuilder<>> Builder;
static std::map<std::string, llvm::Value *> NamedValues;

static void InitializeModule() {
  // Open a new context and module.
  TheContext = std::make_shared<llvm::LLVMContext>();
  TheModule = std::make_shared<llvm::Module>("my cool jit", *TheContext);

  // Create a new builder for the module.
  Builder = std::make_shared<llvm::IRBuilder<>>(*TheContext);
}

ast::AstModule::AstModule(
    const std::shared_ptr<parser::SExpr>& sExpr,
    std::string moduleName)
    : mModuleName(std::move(moduleName))
{
  using namespace std;
  sExpr->forEachInList([this](const shared_ptr<parser::Atom> &atom) {
    if (atom->type() == parser::Atom::Type::sExpr) {
      shared_ptr<parser::SExpr> expr = static_pointer_cast<parser::AtomSExpr>(
          atom)->mExpr;
      mFunctionDefinitions.emplace_back(make_shared<AstFnDef>(expr));
    }
  });
}

std::shared_ptr<llvm::Module> ast::AstModule::codegen() const
{
  using namespace std;

  InitializeModule();

  for (const auto &fn: mFunctionDefinitions) {
    fn->codegen();
  }

  return TheModule;
}

ast::AstFnDef::AstFnDef(const std::shared_ptr<parser::SExpr>& sExpr)
{
  using namespace std;
  // keyword 'fn'
  auto optionalFnKeyword = (*sExpr)[0];
  if (optionalFnKeyword) {
    auto atom = *optionalFnKeyword;
    if (atom->type() == parser::Atom::Type::token) {
      auto atomToken = std::static_pointer_cast<parser::AtomToken>(atom);
      auto token = atomToken->mToken;
      if (token->mType == lexer::TokenType::keyword) {
        if (token->mSrcString == "fn") {
          // All good
        } else {
          throw runtime_error("Expected keyword fn, got another keyword: " + token->mSrcString);
        }
      } else {
        throw runtime_error("Expected keyword, got " + token->description());
      }
    } else {
      throw runtime_error("Expected atom, got list for first element of function.");
    }
  } else {
    throw runtime_error("Expected function definition, got empty list.");
  }

  auto optionalFnName = (*sExpr)[1];
  if (optionalFnName) {
    auto atom = *optionalFnName;
    if (atom->type() == parser::Atom::Type::token) {
      auto atomToken = std::static_pointer_cast<parser::AtomToken>(atom);
      auto token = atomToken->mToken;
      if (token->mType == lexer::TokenType::identifier) {
        mFnName = token->mSrcString;
      } else {
        throw runtime_error("Expected identifier for function name, got " + token->description());
      }
    } else {
      throw runtime_error("Expected atom, got list for second element of function.");
    }
  } else {
    throw runtime_error("??Expected function definition, got empty list.");
  }

  auto optionalParams = (*sExpr)[2];
  if (optionalParams) {
    auto atom = *optionalParams;
    if (atom->type() == parser::Atom::Type::sExpr) {
      auto sExprAtom = std::static_pointer_cast<parser::AtomSExpr>(atom);
      auto paramSExpr = sExprAtom->mExpr;
      paramSExpr->forEachInList([this](const shared_ptr<parser::Atom> &param){
        if (param->type() == parser::Atom::Type::token) {
          auto paramAtomToken = static_pointer_cast<parser::AtomToken>(param);
          mParams.emplace_back(make_shared<AstIdentifier>(paramAtomToken->mToken));
        }
      });
    } else {
      throw runtime_error("Expected parameter list, got atom for third element of function.");
    }
  } else {
    throw runtime_error("??Expected function definition, got empty list.");
  }
}

llvm::Function *ast::AstFnDef::codegen() const
{
  std::vector<llvm::Type *> ArgTypes;

  llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getVoidTy(*TheContext),
      ArgTypes,
      false);
  llvm::Function *TheFunction = llvm::Function::Create(FT,
      llvm::Function::ExternalLinkage,
      mFnName.c_str(),
      *TheModule);

  llvm::BasicBlock *bb = llvm::BasicBlock::Create(*TheContext, "", TheFunction);

  Builder->SetInsertPoint(bb);

  llvm::Constant *retVal = llvm::ConstantInt::get(*TheContext,
      llvm::APInt(64, 192651));
  Builder->CreateRet(retVal);

  llvm::verifyFunction(*TheFunction);

  return TheFunction;
}

ast::AstIdentifier::AstIdentifier(const std::shared_ptr<lexer::Token>& sExpr)
{

}
