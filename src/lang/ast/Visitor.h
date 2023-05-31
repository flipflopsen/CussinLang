#ifndef VISITOR_H
#define VISITOR_H

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"

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
class StructExprAST;
class ReturnExprAST;
class ScopeExprAST;

using namespace llvm;


class Visitor
{
public:
	virtual Value* visit(NumberExprAST* ast) = 0;
	virtual Value* visit(VariableExprAST* ast) = 0;
	virtual Value* visit(BinaryExprAST* ast) = 0;
	virtual Value* visit(CallExprAST* ast) = 0;
	virtual Value* visit(IfExprAST* ast) = 0;
	virtual Value* visit(ForExprAST* ast) = 0;
	virtual Value* visit(UnaryExprAST* ast) = 0;
	virtual Value* visit(LetExprAST* ast) = 0;
	virtual Value* visit(StructExprAST* ast) = 0;
	virtual Value* visit(ReturnExprAST* ast) = 0;
	virtual Value* visit(ScopeExprAST* ast) = 0;

	virtual Function* visit(PrototypeAST* ast) = 0;
	virtual Function* visit(FunctionAST* ast) = 0;
	//virtual void visit(DoExprAST* ast) = 0;
	//virtual void visit(EnumExprAST* ast) = 0;
};

#endif