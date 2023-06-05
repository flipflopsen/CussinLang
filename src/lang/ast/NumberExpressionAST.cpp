#include "headers/CodegenVisitor.h"
#include "headers/NumberExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* NumberExprAST::codegen()
{
	printf("[CODEGEN] Performing code generation for NumberExprAST.\n");
	return GetNumValueFromDataType(&dt, Val);
}