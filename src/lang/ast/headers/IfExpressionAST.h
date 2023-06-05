#ifndef IFEXPRAST_H
#define IFEXPRAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

/// IfExprAST - Class for if expressions
class IfExprAST : public ExprAST
{
	std::unique_ptr<ExprAST> Cond, Then, Else;

public:
	IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
		std::unique_ptr<ExprAST> Else)
		: Cond(std::move(Cond)), Then(std::move(Then)),
		Else(std::move(Else)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

#endif // !IFEXPRAST_H
