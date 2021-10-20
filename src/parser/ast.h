#pragma once

#include <parser.h>

#include <memory>
#include <utility>

namespace parser {
class SExpr;
class Atom;
class AtomSExpr;
class AstFnDef;

class Ast {

};

class AstModule {
public:
    std::string mModuleName;
    std::vector<std::shared_ptr<AstFnDef>> mFunctionDefinitions;

    AstModule(std::shared_ptr<parser::SExpr> sExpr, std::string moduleName) : mModuleName(std::move(moduleName))
    {
        using namespace std;
        sExpr->forEachInList([this](const shared_ptr<Atom>& atom){
            if (atom->type() == Atom::Type::sExpr) {
                shared_ptr<SExpr> expr = static_pointer_cast<AtomSExpr>(atom)->mExpr;
                mFunctionDefinitions.emplace_back(make_shared<AstFnDef>(expr));
            }
        });
    }
};

class AstFnDef {
public:
    explicit AstFnDef(std::shared_ptr<parser::SExpr> sExpr)
    {

    }
};

class AstBinExpr {

};

Ast parseTreeToAst(SExpr sExpr);

}