#include "headers/CodegenVisitor.h"
#include "headers/VariableExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* VariableExprAST::codegen()
{
	printf("[CODEGEN] Performing code generation for VariableExprAST.\n");
	AllocaInst* A = scopeManager.getVariableFromCurrentScope(Name);

	if (!A)
		LogErrorV("Unknown variable name");

	return Builder->CreateLoad(A->getAllocatedType(), A, Name.c_str());
}