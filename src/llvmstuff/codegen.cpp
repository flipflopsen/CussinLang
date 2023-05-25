#include "codegen.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
//#include "../jit/CussinJIT.h"
#include "../lang/parser.h"
#include <map>

using namespace llvm;

void LogError(const char* Str) {
	fprintf(stderr, "[CODEGEN-ERROR] %s\n", Str);
}

Value* LogErrorV(const char* Str) {
	LogError(Str);
	return nullptr;
}


static std::unique_ptr<LLVMContext> TheContext;
static std::unique_ptr<Module> TheModule;
static std::unique_ptr<IRBuilder<>> Builder;
static std::map<std::string, Value*> NamedValues;
static std::unique_ptr<legacy::FunctionPassManager> TheFPM;
static std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
//static std::unique_ptr<orc::CussinJIT> TheJIT;
static ExitOnError ExitOnErr;

Function* getFunction(std::string Name) {
	CodegenVisitor visitor;
	// First, see if the function has already been added to the current module.
	if (auto* F = TheModule->getFunction(Name))
		return F;

	// If not, check whether we can codegen the declaration from some existing
	// prototype.
	auto FI = FunctionProtos.find(Name);
	if (FI != FunctionProtos.end())
		return FI->second->accept(&visitor);

	// If no existing prototype exists, return null.
	return nullptr;
}


// Visitor implementations

Value *CodegenVisitor::visit(NumberExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting NumberExprAST\n");
	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value *CodegenVisitor::visit(VariableExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting VariableExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value *CodegenVisitor::visit(BinaryExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting BinaryExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value *CodegenVisitor::visit(CallExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting CallExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Function *CodegenVisitor::visit(PrototypeAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting PrototypeAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Function *CodegenVisitor::visit(FunctionAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting FunctionAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(IfExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting IfExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}
Value* CodegenVisitor::visit(ForExprAST* ast)
{
	//printf("[CODEGEN] CodegenVisitor is visiting IfExprAST\n");

	auto ret = ast->codegen();
	//TheModule->print(errs(), nullptr);
	return ret;
}

// Codegen implementations

Value *NumberExprAST::codegen()
{ 
	printf("[CODEGEN] Performing code generation for NumberExprAST.\n");
	return ConstantInt::get(*TheContext, APInt(64, Val));
}

Value *VariableExprAST::codegen()
{
	printf("[CODEGEN] Performing code generation for VariableExprAST.\n");
	Value* V = NamedValues[Name];
	if (!V)
		LogErrorV("Unknown variable name");
	return V;
}

Value *BinaryExprAST::codegen()
{
	CodegenVisitor visitor;

	printf("[CODEGEN] Performing code generation for BinaryExprAST.\n");
	Value* L = LHS->accept(&visitor);
	Value* R = RHS->accept(&visitor);
	if (!L || !R)
	{
		printf("Returning nullptr\n");
		return nullptr;
	}

	switch (Op) {
	case '+':
		return Builder->CreateAdd(L, R, "addtmp");
	case '-':
		return Builder->CreateSub(L, R, "subtmp");
	case '*':
		return Builder->CreateMul(L, R, "multmp");
	case '<':
		L = Builder->CreateICmpSLT(L, R, "cmptmp");
		L = Builder->CreateSExt(L, IntegerType::get(*TheContext, 64), "booltmp");
		// Convert bool 0/1 to double 0.0 or 1.0
		//return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
		return L;
	default:
		break;
	}

	// If it wasn't a builtin binary operator, it must be a user defined one. Emit
	// a call to it.
	Function* F = getFunction(std::string("binary") + Op);
	assert(F && "binary operator not found!");

	Value* Ops[2] = { L, R };
	return Builder->CreateCall(F, Ops, "binop");
}

Value *CallExprAST::codegen()
{
	CodegenVisitor visitor;

	printf("[CODEGEN] Performing code generation for CallExprAST.\n");
	Function* CalleeF = TheModule->getFunction(Callee);
	if (!CalleeF)
		return LogErrorV("Unknown function referenced");

	// If argument mismatch error.
	if (CalleeF->arg_size() != Args.size())
		return LogErrorV("Incorrect # arguments passed");

	std::vector<Value*> ArgsV;
	for (unsigned i = 0, e = Args.size(); i != e; ++i) {
		ArgsV.push_back(Args[i]->accept(&visitor));
		if (!ArgsV.back())
			return nullptr;
	}

	return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}

Function *PrototypeAST::codegen()
{
	printf("[CODEGEN] Performing code generation for PrototypeAST.\n");

	Type* returnType = Type::getInt64Ty(*TheContext);

	std::vector<Type*> paramTypes;
	for (const auto& arg : Args) {
		// Assuming all parameter types are i64 for this example
		paramTypes.push_back(llvm::Type::getInt64Ty(*TheContext));
	}


	// Create the function type
	FunctionType* functionType = FunctionType::get(returnType, paramTypes, false);

	// Create the function
	Function* function = Function::Create(functionType, Function::ExternalLinkage, Name, TheModule.get());

	// Set names for the function parameters
	llvm::Function::arg_iterator argIterator = function->arg_begin();
	for (const auto& arg : Args) {
		argIterator->setName(arg);
		++argIterator;
	}

	TheModule->print(errs(), nullptr);

	return function;
}

Function *FunctionAST::codegen()
{
	printf("[CODEGEN] Performing code generation for FunctionAST.\n");

	// Transfer ownership of the prototype to the FunctionProtos map, but keep a
  // reference to it for use below.
	auto& P = *Proto;
	FunctionProtos[Proto->getName()] = std::move(Proto);
	Function* TheFunction = getFunction(P.getName());
	if (!TheFunction)
		return nullptr;

	if (P.isBinaryOp())
		Parser::BinopPrecedence[P.getOperatorName()] = P.getBinaryPrecedence();

	/*
	// Create a new basic block to start insertion into.
	BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", TheFunction);

	CodegenVisitor visitor;

	
	// Create a new LLVM module
	Function* TheFunction = TheModule->getFunction(Proto->getName());

	if (!TheFunction)
		TheFunction = Proto->accept(&visitor);

	if (!TheFunction)
		return nullptr;

	if (!TheFunction->empty())
		return static_cast<Function*>(LogErrorV("Function cannot be redefined."));
		*/


	// Create a new basic block in the function
	BasicBlock* basicBlock = BasicBlock::Create(*TheContext, "entry", TheFunction);

	Builder->SetInsertPoint(basicBlock);

	// Record the function arguments in the NamedValues map.
	NamedValues.clear();

	int ctr = 0;
	for (auto& Arg : TheFunction->args())
	{
		NamedValues[std::string(Arg.getName())] = &Arg;
		ctr++;
	}
	printf("[CODEGEN] Added %d NamedValues.\n", ctr);

	CodegenVisitor visitor;

	// Generate the code for the body expression
	if (Value* RetVal = Body->accept(&visitor)) {
		// Finish off the function.
		printf("[CODEGEN] Finishing off function!\n");
		if (Builder != nullptr)
		{
			Builder->CreateRet(RetVal);

			if (TheFunction != nullptr)
			{
				if (!verifyFunction(*TheFunction))
				{
					printf("[CODEGEN-VERIFIED] Function is ok!\n");
					TheFPM->run(*TheFunction);
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

	TheModule->print(errs(), nullptr);
	// TODO: Better error handling
	TheFunction->eraseFromParent();

	if (P.isBinaryOp())
		Parser::BinopPrecedence.erase(P.getOperatorName());

	return nullptr;
}

Value* IfExprAST::codegen()
{
	CodegenVisitor visitor;
	Value* CondV = Cond->accept(&visitor);

	if (!CondV)
		return nullptr;

	// return ConstantInt::get(*TheContext, APInt(64, Val));
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

Value* ForExprAST::codegen()
{
	CodegenVisitor visitor;

	// Emot the start code first, without variable in scope
	Value* StartVal = Start->accept(&visitor);
	if (!StartVal)
		return nullptr;

	// Make new basic block for the loop header, inserting after current block

	Function* TheFunction = Builder->GetInsertBlock()->getParent();
	BasicBlock* PreheaderBB = Builder->GetInsertBlock();
	BasicBlock* LoopBB =
		BasicBlock::Create(*TheContext, "loop", TheFunction);

	// Insert an explicit fall through from the current block to the LoopBB.
	Builder->CreateBr(LoopBB);

	// Start insertion in LoopBB.
	Builder->SetInsertPoint(LoopBB);

	// Start the PHI node with an entry for Start.
	PHINode* Variable = Builder->CreatePHI(Type::getInt64Ty(*TheContext),
		2, VarName);
	Variable->addIncoming(StartVal, PreheaderBB);

	// Within the loop, the variable is defined equal to the PHI node.  If it
	// shadows an existing variable, we have to restore it, so save it now.
	Value* OldVal = NamedValues[VarName];
	NamedValues[VarName] = Variable;

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
	else {
		// If not specified, use 1.0.
		StepVal = ConstantInt::get(*TheContext, APInt(64, 1));
	}

	Value* NextVar = Builder->CreateAdd(Variable, StepVal, "nextvar");

	// Compute the end condition.
	Value* EndCond = End->accept(&visitor);
	if (!EndCond)
		return nullptr;

	// Convert condition to a bool by comparing non-equal to 0.0.
	EndCond = Builder->CreateICmpNE(
		EndCond, ConstantInt::get(*TheContext, APInt(64, 0)), "loopcond");

	// Create the "after loop" block and insert it.
	BasicBlock* LoopEndBB = Builder->GetInsertBlock();
	BasicBlock* AfterBB =
		BasicBlock::Create(*TheContext, "afterloop", TheFunction);

	// Insert the conditional branch into the end of LoopEndBB.
	Builder->CreateCondBr(EndCond, LoopBB, AfterBB);

	// Any new code will be inserted in AfterBB.
	Builder->SetInsertPoint(AfterBB);

	// Add a new entry to the PHI node for the backedge.
	Variable->addIncoming(NextVar, LoopEndBB);

	// Restore the unshadowed variable.
	if (OldVal)
		NamedValues[VarName] = OldVal;
	else
		NamedValues.erase(VarName);

	// for expr always returns 0.0.
	return Constant::getNullValue(Type::getInt64Ty(*TheContext));
}


void InitializeJIT()
{
	//TheJIT = ExitOnErr(orc::CussinJIT::Create());
}

void InitializeModule(bool optimizations) {
	// Open a new context and module.
	TheContext = std::make_unique<LLVMContext>();
	TheModule = std::make_unique<Module>("cussinJIT", *TheContext);
	//TheModule->setDataLayout(TheJIT->getDataLayout());
	TheFPM = std::make_unique<legacy::FunctionPassManager>(TheModule.get());

	if (optimizations)
	{
		// Optimizations
		// "peephole" optimizations and bit-twiddling.
		TheFPM->add(createInstructionCombiningPass());
		// Reassociate expressions.
		TheFPM->add(createReassociatePass());
		// Eliminate Common SubExpressions.
		TheFPM->add(createGVNPass());
		// Simplify the control flow graph (deleting unreachable blocks, etc).
		TheFPM->add(createCFGSimplificationPass());

	}

	TheFPM->doInitialization();

	// Create a new builder for the module.
	Builder = std::make_unique<IRBuilder<>>(*TheContext);

}
