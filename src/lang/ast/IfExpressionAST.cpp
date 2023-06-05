#include "headers/CodegenVisitor.h"
#include "headers/IfExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* IfExprAST::codegen()
{
	CodegenVisitor visitor;
	Value* CondV = Cond->accept(&visitor);

	if (!CondV)
		return nullptr;

	CondV = Builder->CreateICmpNE(CondV, ConstantInt::get(*TheContext, APInt(64, 0)), "ifcond");

	Function* TheFunction = Builder->GetInsertBlock()->getParent();

	// Create blocks for the then and else cases.  Insert the 'then' block at the
	// end of the function.
	BasicBlock* ThenBB =
		BasicBlock::Create(*TheContext, "then", TheFunction);
	BasicBlock* ElseBB = BasicBlock::Create(*TheContext, "else");
	BasicBlock* MergeBB = BasicBlock::Create(*TheContext, "ifcont");

	Builder->CreateCondBr(CondV, ThenBB, ElseBB);

	// Emit then value.
	Builder->SetInsertPoint(ThenBB);

	Value* ThenV = Then->accept(&visitor);
	if (!ThenV)
		return nullptr;

	Builder->CreateBr(MergeBB);
	// Codegen of 'Then' can change the current block, update ThenBB for the PHI.
	ThenBB = Builder->GetInsertBlock();

	// Emit else block.
	TheFunction->insert(TheFunction->end(), ElseBB);
	Builder->SetInsertPoint(ElseBB);

	Value* ElseV = Else->accept(&visitor);
	if (!ElseV)
		return nullptr;

	Builder->CreateBr(MergeBB);
	// codegen of 'Else' can change the current block, update ElseBB for the PHI.
	ElseBB = Builder->GetInsertBlock();

	// Emit merge block.
	TheFunction->insert(TheFunction->end(), MergeBB);
	Builder->SetInsertPoint(MergeBB);

	PHINode* PN =
		Builder->CreatePHI(Type::getInt64Ty(*TheContext), 2, "iftmp");

	PN->addIncoming(ThenV, ThenBB);
	PN->addIncoming(ElseV, ElseBB);
	return PN;

}