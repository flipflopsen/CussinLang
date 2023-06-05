#include "headers/CodegenVisitor.h"
#include "headers/ReturnExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* ReturnExprAST::codegen()
{
	printf("[CODEGEN] Performing code generation for ReturnAST.\n");

	CodegenVisitor visitor;

	Value* Result = ReturnValue->accept(&visitor);
	if (!Result)
		return nullptr;

	// Emit the return instruction
	Builder->CreateRet(Result);
	return Result;
}