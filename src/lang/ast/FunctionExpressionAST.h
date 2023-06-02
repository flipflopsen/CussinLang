#ifndef FUNCAST_H
#define FUNCAST_H

#include "ExpressionAST.h"
#include "Visitor.h"

class FunctionAST : public ExprAST
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