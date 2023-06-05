#include "headers/CodegenVisitor.h"
#include "headers/BinaryExpressionAST.h"
#include "headers/CallExpressionAST.h"
#include "headers/ForExpressionAST.h"
#include "headers/FunctionExpressionAST.h"
#include "headers/IfExpressionAST.h"
#include "headers/LetExpressionAST.h"
#include "headers/NumberExpressionAST.h"
#include "headers/PrototypeExpressionAST.h"
#include "headers/ReturnExpressionAST.h"
#include "headers/ScopeExpressionAST.h"
#include "headers/StructExpressionAST.h"
#include "headers/UnaryExpressionAST.h"
#include "headers/VariableExpressionAST.h"

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
	//printf("[CODEGEN] CodegenVisitor is visiting ReturnExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(ScopeExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting ScopetExprAST\n");

	ast->codegen();
	//TheModule->print(errs(), nullptr);
	return nullptr;
}