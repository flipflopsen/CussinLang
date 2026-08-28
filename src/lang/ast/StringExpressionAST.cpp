#include "headers/CodegenVisitor.h"
#include "headers/StringExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* StringExprAST::codegen()
{
	printf("[CODEGEN] Performing code generation for StringExprAST.\n");

	Value* str = Builder->CreateGlobalString(Val);

	return str;
}