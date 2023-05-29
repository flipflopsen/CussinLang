#ifndef FORAST_H
#define FORAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

class ForExprAST : public ExprAST
{
	std::string VarName;
	std::unique_ptr<ExprAST> Start, End, Step, Body;
	DataType LoopVarDataType;

public:
	ForExprAST(const std::string& VarName, DataType LoopVarDataType, std::unique_ptr<ExprAST> Start,
		std::unique_ptr<ExprAST> End, std::unique_ptr<ExprAST> Step,
		std::unique_ptr<ExprAST> Body)
		: VarName(VarName), LoopVarDataType(LoopVarDataType), Start(std::move(Start)),
		End(std::move(End)), Step(std::move(Step)),
		Body(std::move(Body)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};


#endif