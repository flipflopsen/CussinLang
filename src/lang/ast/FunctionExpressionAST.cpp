#include "headers/FunctionExpressionAST.h"
#include "headers/CodegenVisitor.h"
#include "../../llvmstuff/codegen.h"

Function *FunctionAST::codegen()
{
	printf("[CODEGEN] Performing code generation for FunctionAST.\n");

	// Transfer ownership of the prototype to the FunctionProtos map, but keep a
	// reference to it for use below.
	auto& P = *Proto;
	scopeManager.addFunctionToCurrentScope(P.getName(), std::move(Proto));
	Function* TheFunction = getFunction(P.getName());

	if (!TheFunction)
		return nullptr;

	if (P.isBinaryOp())
		BinopPrecedence[P.getOperatorName()] = P.getBinaryPrecedence();



	// Create a new basic block in the function
	BasicBlock* basicBlock = BasicBlock::Create(*TheContext, "entry", TheFunction);

	Builder->SetInsertPoint(basicBlock);

	// Record the function arguments in the NamedValues map.

	int ctr = 0;
	for (auto& Arg : TheFunction->args())
	{
		// Create an alloca for this variable.
		AllocaInst* Alloca = CreateEntryBlockAlloca(TheFunction, Arg.getName().str(), Arg.getType());

		// Store the initial value into the alloca.
		Builder->CreateStore(&Arg, Alloca);

		// Add arguments to variable symbol table.
		//symbolTable.addVariable(std::string(Arg.getName()), Alloca);
		scopeManager.addVariableToCurrentScope(std::string(Arg.getName()), Alloca);


		//NamedValues[std::string(Arg.getName())] = &Arg;
		ctr++;
	}
	printf("[CODEGEN] Added %d NamedValues.\n", ctr);

	CodegenVisitor visitor;

	// Generate the code for each expression in the body
	for (const auto& Expr : Body)
	{
		if (Value* RetVal = Expr->accept(&visitor))
		{
			// Check if any error occurred during code generation
			if (RetVal == nullptr)
			{
				LogError("Caught some codegen-fn error because accept returned nullptr");
				TheFunction->eraseFromParent();
				scopeManager.removeFunctionFromScope(true, P.getName());
				return nullptr;
			}
		}
		else
		{
			LogError("Caught some codegen-fn error (IN ELSE) because accept returned nullptr ");
			TheFunction->eraseFromParent();
			scopeManager.removeFunctionFromScope(true, P.getName());
			return nullptr;
		}
	}

	// Finish off the function.
	printf("[CODEGEN] Finishing off function!\n");
	if (Builder != nullptr)
	{
		//Builder->CreateRetVoid();

		if (TheFunction != nullptr)
		{
			if (!verifyFunction(*TheFunction))
			{
				printf("[CODEGEN-VERIFIED] Function is ok!\n");
				TheFPM->run(*TheFunction);
			}
			else
			{
				LogError("Failed to verify function!");
			}
		}
		else
		{
			printf("[CODEGEN-ERR] TheFunction is null!\n");
		}
	}
	else
	{
		printf("[CODEGEN-ERR] BuilderPtr is null!\n");
	}

	return TheFunction;
}