#ifndef AST2_H
#define AST2_H


#include "BinaryExpressionAST.h"
#include "CallExpressionAST.h"
#include "ForExpressionAST.h"
#include "FunctionExpressionAST.h"
#include "IfExpressionAST.h"
#include "LetExpressionAST.h"
#include "NumberExpressionAST.h"
#include "PrototypeExpressionAST.h"
#include "UnaryExpressionAST.h"
#include "VariableExpressionAST.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"

#include "Visitor.h"

class CodegenVisitor : public Visitor
{
public:
	llvm::Value* visit(NumberExprAST* ast) override;
	llvm::Value* visit(VariableExprAST* ast) override;
	llvm::Value* visit(BinaryExprAST* ast) override;
	llvm::Value* visit(CallExprAST* ast) override;
	llvm::Value* visit(IfExprAST* ast) override;
	llvm::Value* visit(ForExprAST* ast) override;
	llvm::Value* visit(UnaryExprAST* ast) override;
	llvm::Value* visit(LetExprAST* ast) override;

	llvm::Function* visit(PrototypeAST* ast) override;
	llvm::Function* visit(FunctionAST* ast) override;
};

#endif