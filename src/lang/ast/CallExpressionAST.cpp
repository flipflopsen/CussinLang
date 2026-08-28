#include "headers/CodegenVisitor.h"
#include "headers/CallExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* CallExprAST::codegen()
{
	printf("[CODEGEN-CALL] Performing code generation for CallExprAST.\n");

	CodegenVisitor visitor;
	Function* CalleeF = getFunction(Callee);

	if (!CalleeF)
		return LogErrorV("Unknown function referenced");

	// If argument mismatch error.
	if (CalleeF->arg_size() != Args.size())
		return LogErrorV("Incorrect # arguments passed");

	std::vector<Value*> ArgsV;
	for (unsigned i = 0, e = Args.size(); i != e; ++i) 
	{
		ArgsV.push_back(Args[i]->accept(&visitor));
		if (!ArgsV.back())
			return nullptr;
	}

	return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}