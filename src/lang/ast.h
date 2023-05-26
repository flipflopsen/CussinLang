#ifndef AST_H
#define AST_H

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <vector>
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"


class ExprAST;
class NumberExprAST;
class VariableExprAST;
class BinaryExprAST;
class CallExprAST;
class PrototypeAST;
class FunctionAST;
class IfExprAST;
class ForExprAST;
class UnaryExprAST;
class LetExprAST;
//class DoExprAST;
//class TryExprAST;
//class StructExprAST;
//class EnumExprAST;

using namespace llvm;

class Visitor
{
public:
	virtual Value *visit(NumberExprAST* ast) = 0;
	virtual Value *visit(VariableExprAST* ast) = 0;
	virtual Value *visit(BinaryExprAST* ast) = 0;
	virtual Value *visit(CallExprAST* ast) = 0;
	virtual Function *visit(PrototypeAST* ast) = 0;
	virtual Function *visit(FunctionAST* ast) = 0;
	virtual Value *visit(IfExprAST* ast) = 0;
	virtual Value* visit(ForExprAST* ast) = 0;
	virtual Value* visit(UnaryExprAST* ast) = 0;
	virtual Value* visit(LetExprAST* ast) = 0;
	//virtual void visit(DoExprAST* ast) = 0;
	//virtual void visit(StructExprAST* ast) = 0;
	//virtual void visit(EnumExprAST* ast) = 0;
};

class CodegenVisitor : public Visitor
{
public:
	Value *visit(NumberExprAST* ast) override;
	Value *visit(VariableExprAST* ast) override;
	Value *visit(BinaryExprAST* ast) override;
	Value *visit(CallExprAST* ast) override;
	Function *visit(PrototypeAST* ast) override;
	Function *visit(FunctionAST* ast) override;
	Value* visit(IfExprAST* ast) override;
	Value* visit(ForExprAST* ast) override;
	Value* visit(UnaryExprAST* ast) override;
	Value* visit(LetExprAST* ast) override;
};


class ExprAST
{
public:
	virtual ~ExprAST() = default;
	virtual Value *accept(Visitor* visitor) = 0;

};

class NumberExprAST : public ExprAST {
	double Val;

public:
	NumberExprAST(double val) : Val(val) {}

	Value *accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value *codegen();
};


/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST
{
	std::string Name;
public:
	VariableExprAST(const std::string& Name) : Name(Name) {}

	Value *accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	const std::string& getName() const{ return Name; }
	Value *codegen();
};


/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST
{
	char Op;
	std::unique_ptr<ExprAST> LHS, RHS;

public:
	BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
		std::unique_ptr<ExprAST> RHS)
		: Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

	Value *accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value *codegen();
};


/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST
{
	std::string Callee;
	std::vector<std::unique_ptr<ExprAST>> Args;

public:
	CallExprAST(const std::string& Callee,
		std::vector<std::unique_ptr<ExprAST>> Args)
		: Callee(Callee), Args(std::move(Args)) {}

	Value *accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value *codegen();
};

/// PrototypeAST - represents a Prototype for a function with name, argument names etc.

class PrototypeAST
{
	std::string Name;
	std::vector<std::string> Args;
	bool IsOperator;
	unsigned Precedence; // Precedence if BinOp

public:
	PrototypeAST(const std::string& Name, std::vector<std::string> Args,
		bool IsOperator = false, unsigned Prec = 0)
		: Name(Name), Args(std::move(Args)),
			IsOperator(IsOperator),Precedence(Prec) {}

	Function *accept(Visitor* visitor) {
		return visitor->visit(this);
	}

	Function *codegen();
	const std::string& getName() const { return Name; }

	bool isUnaryOp() const { return IsOperator && Args.size() == 1; }
	bool isBinaryOp() const { return IsOperator && Args.size() == 2; }

	char getOperatorName() const {
		assert(isUnaryOp() || isBinaryOp());
		return Name[Name.size() - 1];
	}

	unsigned getBinaryPrecedence() const { return Precedence; }
};


class FunctionAST
{
	std::unique_ptr<PrototypeAST> Proto;
	std::unique_ptr<ExprAST> Body;

public:
	FunctionAST(std::unique_ptr<PrototypeAST> Proto,
		std::unique_ptr<ExprAST> Body)
		: Proto(std::move(Proto)), Body(std::move(Body)) {}

	Function *accept(Visitor* visitor) {
		return visitor->visit(this);
	}

	Function *codegen();
};


class IfExprAST : public ExprAST
{
	std::unique_ptr<ExprAST> Cond, Then, Else;

public:
	IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
		std::unique_ptr<ExprAST> Else)
		: Cond(std::move(Cond)), Then(std::move(Then)),
			Else(std::move(Else)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

class ForExprAST : public ExprAST
{
	std::string VarName;
	std::unique_ptr<ExprAST> Start, End, Step, Body;

public:
	ForExprAST(const std::string &VarName, std::unique_ptr<ExprAST> Start, 
		std::unique_ptr<ExprAST> End, std::unique_ptr<ExprAST> Step, 
		std::unique_ptr<ExprAST> Body)
			: VarName(VarName), Start(std::move(Start)),
				End(std::move(End)), Step(std::move(Step)),
				Body(std::move(Body)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

class UnaryExprAST : public ExprAST
{
	char Opcode;
	std::unique_ptr<ExprAST> Operand;

public:
	UnaryExprAST(char Opcode, std::unique_ptr<ExprAST> Operand)
		: Opcode(Opcode), Operand(std::move(Operand)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

class LetExprAST : public ExprAST
{
	std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames;
	std::unique_ptr<ExprAST> Body;

public:
	LetExprAST(std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames,
		std::unique_ptr<ExprAST> Body)
		: VarNames(std::move(VarNames)), Body(std::move(Body)) {}

	Value* accept(Visitor* visitor) override {
		return visitor->visit(this);
	}

	Value* codegen();
};

/*
class DoExprAST : public ExprAST {

};

class TryExprAST : public ExprAST {

};


class StructExprAST : public ExprAST {
	std::string Name;
	std::vector<std::unique_ptr<ExprAST>> Contents;

public:
	StructExprAST(std::string Name, std::vector<std::unique_ptr<ExprAST>> Contents)
		: Name(Name), Contents(std::move(Contents)) {}
	llvm::Value* codegen() override;
};

class EnumExprAST : public ExprAST {
	std::string Name;
	std::vector<std::unique_ptr<ExprAST>> Contents;

public:
	EnumExprAST(std::string Name, std::vector<std::unique_ptr<ExprAST>> Contents)
		: Name(Name), Contents(std::move(Contents)) {}
	llvm::Value* codegen() override;
};
*/

#endif