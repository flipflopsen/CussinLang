#ifndef VISITORS_H
#define VISITORS_H


#include "codegen.h"

// Visitor implementations

Value* CodegenVisitor::visit(NumberExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting NumberExprAST\n");
	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(VariableExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting VariableExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(BinaryExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting BinaryExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(CallExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting CallExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Function* CodegenVisitor::visit(PrototypeAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting PrototypeAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Function* CodegenVisitor::visit(FunctionAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting FunctionAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(IfExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting IfExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(ForExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting ForExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(UnaryExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting UnaryExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(LetExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting LetExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(StructExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting StructExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(ReturnExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting StructExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}

#endif