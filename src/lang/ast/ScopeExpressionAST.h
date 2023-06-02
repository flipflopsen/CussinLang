#ifndef SCOPEEXPRAST_H
#define SCOPEEXPRAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

class ScopeExprAST : public ExprAST {
private:
    std::string Name;
    bool IsPersistent;
    std::vector<std::unique_ptr<ExprAST>> Body;

public:
    ScopeExprAST(const std::string& Name, bool IsPersistent, std::vector<std::unique_ptr<ExprAST>> Body)
        : Name(Name), IsPersistent(IsPersistent), Body(std::move(Body)) {}

    Value* accept(Visitor* visitor) override {
        visitor->visit(this);
        return nullptr;
    }

    Value* codegen();
};

#endif