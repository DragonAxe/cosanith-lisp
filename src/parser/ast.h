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

class Ast
{
public:
};

class AstModule : public Ast
{
public:
  std::string mModuleName;
  std::vector<std::shared_ptr<AstFnDef>> mFunctionDefinitions;

  AstModule(std::shared_ptr<parser::SExpr> sExpr, std::string moduleName);

  [[nodiscard]] std::shared_ptr<llvm::Module> codegen() const;
};

class AstFnDef : public Ast
{
public:
  std::string mFnName;
  std::vector<std::string> mParamNames;
  std::vector<std::string> mParamTypes;

  explicit AstFnDef(std::shared_ptr<parser::SExpr> sExpr);

  llvm::Function *codegen() const;
};

class AstBinExpr : public Ast
{

};

}
