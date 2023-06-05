#ifndef UNARYAST_H
#define UNARYAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

class UnaryExprAST : public ExprAST
{
	char Opcode;
	std::unique_ptr<ExprAST> Operand;

public:
	UnaryExprAST(char Opcode, std::unique_ptr<ExprAST> Operand)
		: Opcode(Opcode), Operand(std::move(Operand)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

#endif