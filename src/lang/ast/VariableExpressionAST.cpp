#include "headers/CodegenVisitor.h"
#include "headers/VariableExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* VariableExprAST::codegen()
{
	auto& scopeManager = ScopeManager::getInstance();

	printf("[CODEGEN] Performing code generation for VariableExprAST.\n");
	AllocaInst* A = scopeManager.getVariable(true, Name);

	if (!A)
		LogErrorV("Unknown variable name");

	return Builder->CreateLoad(A->getAllocatedType(), A, Name.c_str());
}