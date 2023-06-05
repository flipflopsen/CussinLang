#ifndef FUNCAST_H
#define FUNCAST_H

#include "../../../utils/BinopPrecedence.h"
#include "PrototypeExpressionAST.h"

/// FunctionAST - Class for function def and decl
class FunctionAST
{
	std::unique_ptr<PrototypeAST> Proto;
	std::vector<std::unique_ptr<ExprAST>> Body;

public:
	FunctionAST(std::unique_ptr<PrototypeAST> Proto,
		std::vector<std::unique_ptr<ExprAST>> Body)
		: Proto(std::move(Proto)), Body(std::move(Body)) {}

	Function* accept(Visitor* visitor) {
		return visitor->visit(this);
		//return visitor->visit(this);
	}

	Function* codegen();
};


#endif