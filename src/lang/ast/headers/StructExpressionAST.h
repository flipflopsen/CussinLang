#ifndef STRUCTEXPRAST_H
#define STRUCTEXPRAST_H

#include "ExpressionAST.h"
#include "Visitor.h"


class StructExprAST : public ExprAST
{
	std::string Typename;
	std::vector<std::pair<std::string , DataType>> Fields;

public:
	StructExprAST(const std::string& Typename, 
		std::vector<std::pair<std::string , DataType>> Fields) 
	: Typename(Typename), Fields(Fields) {}


	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	const std::string& getTypeName() const { return Typename; }
	Value* codegen();
};

#endif