#ifndef CALLAST_H
#define CALLAST_H

#include "Visitor.h"
#include "ExpressionAST.h"

/// CallExprAST - Expression class for function calls
class CallExprAST : public ExprAST
{
	std::string Callee;
	std::vector<std::unique_ptr<ExprAST>> Args;

public:
	CallExprAST(const std::string& Callee,
		std::vector<std::unique_ptr<ExprAST>> Args)
		: Callee(Callee), Args(std::move(Args)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

// Codegen implementations

#endif