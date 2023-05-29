#ifndef NUMBERAST_H
#define NUMBERAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

class NumberExprAST : public ExprAST {
	double Val;

public:
	NumberExprAST(double val) : Val(val) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

#endif