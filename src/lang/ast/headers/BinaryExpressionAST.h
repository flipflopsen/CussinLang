#ifndef BINARYAST_H
#define BINARYAST_H

#include "ExpressionAST.h"

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST
{
	char Op;
	std::unique_ptr<ExprAST> LHS, RHS;

public:
	BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
		std::unique_ptr<ExprAST> RHS)
		: Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

#endif