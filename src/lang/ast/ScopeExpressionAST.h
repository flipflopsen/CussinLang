#ifndef SCOPEEXPRAST_H
#define SCOPEEXPRAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

class ScopeExprAST : public ExprAST
{
private:
	std::string Name;
	bool isPersistent;

public:
	ScopeExprAST(const std::string& Name, bool IsPersistent)
		: ReturnValue(std::move(ReturnValue)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

#endif