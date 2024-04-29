#include "headers/CodegenVisitor.h"
#include "headers/ForExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* ForExprAST::codegen()
{
	auto& scopeManager = ScopeManager::getInstance();

	printf("[CODEGEN] Performing code generation for ForAST.\n");

	CodegenVisitor visitor;

	// Get TheFunction
	Function* TheFunction = Builder->GetInsertBlock()->getParent();

	// Create a new alloca for the var in entry block
	AllocaInst* Alloca = CreateEntryBlockAlloca(TheFunction, VarName, GetLLVMTypeFromDataType(&this->LoopVarDataType));

	// Emit start code without var in scope
	Value* StartVal = Start->accept(&visitor);
	if (!StartVal)
		return nullptr;

	// Store the value into the alloca
	Builder->CreateStore(StartVal, Alloca);

	// Make new basic block for the loop header, inserting after current block
	BasicBlock* LoopBB = BasicBlock::Create(*TheContext, "loop", TheFunction);

	// Insert an explicit fall through from the current block to the LoopBB.
	Builder->CreateBr(LoopBB);

	// Start insertion in LoopBB.
	Builder->SetInsertPoint(LoopBB);


	AllocaInst* OldVal = scopeManager.getVariable(true, VarName);
	scopeManager.addVariable(true, VarName, Alloca);


	// Emit the body of the loop.  This, like any other expr, can change the
	// current BB.  Note that we ignore the value computed by the body, but don't
	// allow an error.
	if (!Body->accept(&visitor))
		return nullptr;

	// Emit the step value.
	Value* StepVal = nullptr;
	if (Step) {
		StepVal = Step->accept(&visitor);
		if (!StepVal)
			return nullptr;
	}
	else
	{
		// If not specified, use 1
		//TODO: 64 bit int bruh
		StepVal = ConstantInt::get(*TheContext, APInt(64, 1));
	}

	// Compute the end condition.
	Value* EndCond = End->accept(&visitor);
	if (!EndCond)
		return nullptr;


	/*
	 * Reload, increment and restore the alloca.
	 * Handles the case where the body of the loop mutates the var
	 */
	Value* CurVar = Builder->CreateLoad(Alloca->getAllocatedType(), Alloca, VarName.c_str());
	Value* NextVar = Builder->CreateAdd(CurVar, StepVal, "nextvar");
	Builder->CreateStore(NextVar, Alloca);


	// Convert condition to a bool by comparing non-equal to 0.0.
	EndCond = Builder->CreateICmpNE(
		EndCond, ConstantInt::get(*TheContext, APInt(64, 0)), "loopcond");

	// Create the "after loop" block and insert it.
	//BasicBlock* LoopEndBB = Builder->GetInsertBlock();
	BasicBlock* AfterBB = BasicBlock::Create(*TheContext, "afterloop", TheFunction);

	// Insert the conditional branch into the end of LoopEndBB.
	Builder->CreateCondBr(EndCond, LoopBB, AfterBB);

	// Any new code will be inserted in AfterBB.
	Builder->SetInsertPoint(AfterBB);

	// Restore the unshadowed variable.
	if (OldVal)
		//symbolTable.addVariable(VarName, OldVal);
		scopeManager.addVariable(true, VarName, OldVal);
	else
		//TODO
		//symbolTable.removeVariable(VarName);
		scopeManager.removeVariable(true, VarName);

	// for expr always returns 0.0.
	//TODO: Int64Ty bruh
	return Constant::getNullValue(Type::getInt64Ty(*TheContext));
}