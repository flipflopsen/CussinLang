#include "headers/CodegenVisitor.h"
#include "headers/UnaryExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* UnaryExprAST::codegen()
{
	printf("[CODEGEN] Performing code generation for UnaryAST.\n");

	CodegenVisitor visitor;
	Value* OperandV = Operand->accept(&visitor);
	if (!OperandV)
		return nullptr;

	Function* F = getFunction(std::string("unary") + Opcode);
	if (!F)
		return LogErrorV("Unknown unary operator");

	return Builder->CreateCall(F, OperandV, "unop");
}