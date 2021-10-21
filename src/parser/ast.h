#pragma once

#include <parser.h>

#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>

#include <memory>
#include <utility>

namespace parser {
class SExpr;

class Atom;

class AtomSExpr;

class AstFnDef;

class Ast
{
public:
  virtual llvm::Value *codegen(
      std::unique_ptr<llvm::LLVMContext> context,
      std::shared_ptr<llvm::Module> module,
      std::unique_ptr<llvm::IRBuilder<>> builder) = 0;
};

class AstModule : public Ast
{
public:
  std::string mModuleName;
  std::vector<std::shared_ptr<AstFnDef>> mFunctionDefinitions;

  AstModule(std::shared_ptr<parser::SExpr> sExpr, std::string moduleName)
      : mModuleName(std::move(moduleName))
  {
    using namespace std;
    sExpr->forEachInList([this](const shared_ptr<Atom> &atom) {
      if (atom->type() == Atom::Type::sExpr) {
        shared_ptr<SExpr> expr = static_pointer_cast<AtomSExpr>(atom)->mExpr;
        mFunctionDefinitions.emplace_back(make_shared<AstFnDef>(expr));
      }
    });
  }

  llvm::Value *codegen(
      std::unique_ptr<llvm::LLVMContext> context,
      std::shared_ptr<llvm::Module> module,
      std::unique_ptr<llvm::IRBuilder<>> builder) override
  {
    return nullptr;
  }
};

class AstFnDef : public Ast
{
public:
  explicit AstFnDef(std::shared_ptr<parser::SExpr> sExpr)
  {

  }

  llvm::Value *codegen(
      std::unique_ptr<llvm::LLVMContext> context,
      std::shared_ptr<llvm::Module> module,
      std::unique_ptr<llvm::IRBuilder<>> builder) override
  {
    std::vector<llvm::Type *> ArgTypes;
    llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context),
        ArgTypes,
        false);
    llvm::Function *TheFunction = llvm::Function::Create(FT,
        llvm::Function::ExternalLinkage,
        "MyFun",
        module.get());

    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*context, "", TheFunction);

    builder->SetInsertPoint(bb);

    llvm::Constant *retVal = llvm::ConstantInt::get(*context,
        llvm::APInt(64, 192651));
    builder->CreateRet(retVal);

    return TheFunction;
  }
};

class AstBinExpr : public Ast
{

};

}
