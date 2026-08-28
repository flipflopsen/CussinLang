#ifndef STRINGAST_H
#define STRINGAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

class StringExprAST : public ExprAST {
	const char* Val;
	DataType dt;

public:
	StringExprAST(const char* val, DataType dt = DT_STRING) : Val(val), dt(dt) {}

	llvm::Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	llvm::Value* codegen();
};

#endif