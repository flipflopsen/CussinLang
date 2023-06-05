#include "headers/BinaryExpressionAST.h"
#include "headers/VariableExpressionAST.h"
#include "headers/CodegenVisitor.h"
#include "../../llvmstuff/codegen.h"

Value* BinaryExprAST::codegen()
{
	fprintf(stderr, "[CODEGEN] Performing code generation for BinaryExprAST.\n");

	CodegenVisitor visitor;


	if (Op == '=')
	{
		fprintf(stderr, "[CODEGEN-BINAST] Encountered '=' operator.\n");
		VariableExprAST* LHSE = static_cast<VariableExprAST*>(LHS.get());
		if (!LHSE)
			return LogErrorV("destination of '=' must be a variable.");
		Value* Val = RHS->accept(&visitor);
		if (!Val)
			return nullptr;

		Value* Variable = scopeManager.getVariableFromCurrentScope(LHSE->getName());
		if (!Variable)
			return LogErrorV("Unknown variable name.");

		Builder->CreateStore(Val, Variable);
		return Val;
	}
	Value* L = LHS->accept(&visitor);
	Value* R = RHS->accept(&visitor);
	if (!L || !R)
	{
		return nullptr;
	}

	//Todo: compare types of L and R and do error handling with casting or sth.

	switch (Op) {
	case '+':
		return Builder->CreateAdd(L, R, "addtmp");
	case '-':
		return Builder->CreateSub(L, R, "subtmp");
	case '*':
		return Builder->CreateMul(L, R, "multmp");
	case '<':
		L = Builder->CreateICmpSLT(L, R, "cmptmp");
		L = Builder->CreateSExt(L, R->getType(), "booltmp");
		// Convert bool 0/1 to int 0 or 1
		//return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
		return L;
	default:
		break;
	}

	// If it wasn't a builtin binary operator, it must be a user defined one, so emit a call to it.
	Function* F = getFunction(std::string("binary") + Op);
	assert(F && "binary operator not found!");

	Value* Ops[2] = { L, R };
	return Builder->CreateCall(F, Ops, "binop");
}
