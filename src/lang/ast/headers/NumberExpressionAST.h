#ifndef NUMBERAST_H
#define NUMBERAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

class NumberExprAST : public ExprAST {
	double Val;
	DataType dt;

public:
	NumberExprAST(double val, DataType dt = DT_I32) : Val(val), dt(dt) {}

	llvm::Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	llvm::Value* codegen();
};

#endif