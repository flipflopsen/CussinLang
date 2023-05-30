#ifndef RETURNEXPRAST_H
#define RETURNEXPRAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

class ReturnExprAST : public ExprAST
{
private:
    std::unique_ptr<ExprAST> ReturnValue;

public:
    ReturnExprAST(std::unique_ptr<ExprAST> ReturnValue)
        : ReturnValue(std::move(ReturnValue)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

#endif