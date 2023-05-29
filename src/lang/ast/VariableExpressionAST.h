#ifndef VariableAST_H
#define VariableAST_H

#include "ExpressionAST.h"
#include "Visitor.h"


class VariableExprAST : public ExprAST
{
	std::string Name;
	DataType dt;
public:
	VariableExprAST(const std::string& Name, DataType dt) : Name(Name), dt(dt) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	const std::string& getName() const { return Name; }
	Value* codegen();
};

#endif