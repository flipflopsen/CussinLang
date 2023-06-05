#ifndef AST2_H
#define AST2_H

#include "llvm/IR/BasicBlock.h"

#include "Visitor.h"

/// CodegenVisitor header file
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
	llvm::Value* visit(StructExprAST* ast) override;
	llvm::Value* visit(ReturnExprAST* ast) override;
	llvm::Value* visit(ScopeExprAST* ast) override;

	llvm::Function* visit(PrototypeAST* ast) override;
	llvm::Function* visit(FunctionAST* ast) override;
};

#endif