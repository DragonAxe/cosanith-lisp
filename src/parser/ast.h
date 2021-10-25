#pragma once

#include <parser.h>

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

#include <memory>
#include <utility>

namespace ast {
class SExpr;

class Atom;

class AtomSExpr;

class AstFnDef;
class AstIdentifier;

class Ast
{
public:
};

class AstModule : public Ast
{
public:
  std::string mModuleName;
  std::vector<std::shared_ptr<AstFnDef>> mFunctionDefinitions;

  AstModule(const std::shared_ptr<parser::SExpr>& sExpr, std::string moduleName);

  [[nodiscard]] std::shared_ptr<llvm::Module> codegen() const;
};

class AstFnDef : public Ast
{
public:
  std::string mFnName;
  std::vector<std::shared_ptr<AstIdentifier>> mParams;

  explicit AstFnDef(const std::shared_ptr<parser::SExpr>& sExpr);

  llvm::Function *codegen() const;
};

class AstIdentifier : public Ast
{
public:
  std::string mName;
  std::string mType;

  explicit AstIdentifier(const std::shared_ptr<lexer::Token>& sExpr);
};

class AstBinExpr : public Ast
{

};

}
