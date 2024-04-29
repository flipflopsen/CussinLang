#ifndef PROTOTYPEEXPRAST_H
#define PROTOTYPEEXPRAST_H

#include "ExpressionAST.h"
#include "Visitor.h"


class PrototypeAST
{
	std::string Name;
	std::vector< std::pair<std::string, DataType>> Args;
	bool IsOperator;
	unsigned Precedence;
	DataType returnType;

public:
	PrototypeAST(const std::string& Name, std::vector <std::pair<std::string, DataType>> Args, DataType returnType,
		bool IsOperator = false, unsigned Prec = 0)
		: Name(Name), Args(std::move(Args)), returnType(returnType),
		IsOperator(IsOperator), Precedence(Prec) {}

	Function* accept(Visitor* visitor) {
		return visitor->visit(this);
	}

	Function* codegen();
	const std::string& getName() const { return Name; }

	bool isUnaryOp() const { return IsOperator && Args.size() == 1; }
	bool isBinaryOp() const { return IsOperator && Args.size() == 2; }

	char getOperatorName() const {
		assert(isUnaryOp() || isBinaryOp());
		return Name[Name.size() - 1];
	}

	unsigned getBinaryPrecedence() const { return Precedence; }

	DataType getReturnType() const { return returnType; }
};

#endif
