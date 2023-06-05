#ifndef LETEXPRAST_H
#define LETEXPRAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

/// LetExprAST - Class for let expressions
class LetExprAST : public ExprAST
{
	std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames;
	std::unique_ptr<ExprAST> Body;
	DataType dt;

public:
	LetExprAST(std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames,
		std::unique_ptr<ExprAST> Body, DataType dt)
		: VarNames(std::move(VarNames)), Body(std::move(Body)), dt(dt) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

#endif