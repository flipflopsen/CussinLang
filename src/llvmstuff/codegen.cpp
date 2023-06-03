#include "codegen.h"
#include "visitors.h"

#include <codecvt>
#include <iostream>
#include <map>

#include "../lang/parser.h"
#include "ScopeManager.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include <llvm/Linker/Linker.h>

using namespace llvm;
using namespace llvm::sys;

AllocaInst* CreateEntryBlockAlloca(Function* TheFunction, const std::string& VarName, Type* type);


void LogError(const char* Str) {
	fprintf(stderr, "[CODEGEN-ERROR] %s\n", Str);
}

Value* LogErrorV(const char* Str) {
	LogError(Str);
	return nullptr;
}


//static std::unique_ptr<orc::CussinJIT> TheJIT;
static std::unique_ptr<LLVMContext> TheContext;
static std::unique_ptr<legacy::FunctionPassManager> TheFPM;
static ExecutionEngine* engine;
std::unique_ptr<Module> TheModule;
Module* ModuleVar;
IRBuilder<>* Builder;
ScopeManager& scopeManager = ScopeManager::getInstance();


Function* getFunction(std::string Name) {
	CodegenVisitor visitor;

	if (auto* F = ModuleVar->getFunction(Name))
		return F;

	PrototypeAST* proto = scopeManager.getFunctionFromCurrentScope(Name);
	if (proto)
		return proto->accept(&visitor);

	// If no existing prototype exists, return null.
	return nullptr;
}


// Codegen implementations

Value *NumberExprAST::codegen()
{ 
	printf("[CODEGEN] Performing code generation for NumberExprAST.\n");
	return GetValueFromDataType(&dt, Val);
}

Value *VariableExprAST::codegen()
{
	printf("[CODEGEN] Performing code generation for VariableExprAST.\n");
	AllocaInst* A = scopeManager.getVariableFromCurrentScope(Name);
	//AllocaInst* A = symbolTable.getVariable(Name);

	if (!A)
		LogErrorV("Unknown variable name");

	//return Builder->CreateLoad(A->getAllocatedType(), A, Name.c_str());
	return Builder->CreateLoad(A->getAllocatedType(), A, Name.c_str());
}

Value *BinaryExprAST::codegen()
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

		//Value* Variable = symbolTable.getVariable(LHSE->getName());
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
		printf("Returning nullptr\n");
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
	//Function* CalleeF = TheModule->getFunction(Callee);
	Function* CalleeF = getFunction(Callee);
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

	//Type* returnType = Type::getInt64Ty(*TheContext);
	Type* returnType = GetLLVMTypeFromDataType(&this->returnType);

	std::vector<Type*> paramTypes;
	for (const auto& arg : Args) {
		// Assuming all parameter types are i64 for this example
		paramTypes.push_back(GetLLVMTypeFromDataType(const_cast<DataType*>(&arg.second)));
		//paramTypes.push_back(llvm::Type::getInt64Ty(*TheContext));
	}


	// Create the function type
	FunctionType* functionType = FunctionType::get(returnType, paramTypes, false);

	// Create the function
	Function* function = Function::Create(functionType, Function::ExternalLinkage, Name, *ModuleVar);

	// Set names for the function parameters
	llvm::Function::arg_iterator argIterator = function->arg_begin();
	for (const auto& arg : Args) {
		argIterator->setName(arg.first);
		++argIterator;
	}

	ModuleVar->print(errs(), nullptr);

	return function;
}

Function *FunctionAST::codegen()
{
	printf("[CODEGEN] Performing code generation for FunctionAST.\n");

	// Transfer ownership of the prototype to the FunctionProtos map, but keep a
	// reference to it for use below.
	auto& P = *Proto;
	scopeManager.addFunctionToCurrentScope(P.getName(), std::move(Proto));
	//symbolTable.addFunction(P.getName(), std::move(Proto));
	Function* TheFunction = getFunction(P.getName());

	if (!TheFunction)
		return nullptr;

	if (P.isBinaryOp())
		Parser::BinopPrecedence[P.getOperatorName()] = P.getBinaryPrecedence();



	// Create a new basic block in the function
	BasicBlock* basicBlock = BasicBlock::Create(*TheContext, "entry", TheFunction);

	Builder->SetInsertPoint(basicBlock);

	// Record the function arguments in the NamedValues map.
	//NamedValues.clear();

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
				TheFunction->eraseFromParent();
				scopeManager.removeFunctionFromScope(true, P.getName());
				return nullptr;
			}
		}
		else
		{
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

Value* IfExprAST::codegen()
{
	CodegenVisitor visitor;
	Value* CondV = Cond->accept(&visitor);

	if (!CondV)
		return nullptr;

	// return ConstantInt::get(*TheContext, APInt(64, Val));
	//TODO: Int64Ty bruh
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

	//TODO: Int64Ty bruh
	PHINode* PN =
		Builder->CreatePHI(Type::getInt64Ty(*TheContext), 2, "iftmp");

	PN->addIncoming(ThenV, ThenBB);
	PN->addIncoming(ElseV, ElseBB);
	return PN;

}

Value* ForExprAST::codegen()
{
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

	/*
	 * Within the loop, the variable is defined equal to the PHI node.
	 * If it shadows an existing variable, we have to restore it, so save it now.
	 */

	//AllocaInst* OldVal = symbolTable.getVariable(VarName);
	//symbolTable.addVariable(VarName, Alloca);

	AllocaInst* OldVal = scopeManager.getVariableFromCurrentScope(VarName);
	scopeManager.addVariableToCurrentScope(VarName, Alloca);


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

	// Add a new entry to the PHI node for the backedge.
	//Variable->addIncoming(NextVar, LoopEndBB);

	// Restore the unshadowed variable.
	if (OldVal)
		//symbolTable.addVariable(VarName, OldVal);
		scopeManager.addVariableToCurrentScope(VarName, OldVal);
	else
		//TODO
		//symbolTable.removeVariable(VarName);
		scopeManager.removeVariableFromCurrentScope(VarName);

	// for expr always returns 0.0.
	//TODO: Int64Ty bruh
	return Constant::getNullValue(Type::getInt64Ty(*TheContext));
}

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

Value* LetExprAST::codegen()
{
	printf("[CODEGEN] Performing code generation for LetAST.\n");
	CodegenVisitor visitor;

	std::vector<AllocaInst*> OldBindings;

	Function* TheFunction = Builder->GetInsertBlock()->getParent();

	// Register all variables and emit their initializer.
	for (unsigned i = 0, e = VarNames.size(); i != e; ++i) {
		const std::string& VarName = VarNames[i].first;
		ExprAST* Init = VarNames[i].second.get();

		// Emit the initializer before adding the variable to scope, this prevents
		// the initializer from referencing the variable itself, and permits stuff
		// like this:
		//  var a = 1 in
		//    var a = a in ...   # refers to outer 'a'.

		Value* InitVal;
		if (Init) 
		{
			InitVal = Init->accept(&visitor);
			if (!InitVal)
				return nullptr;
		}
		else 
		{ // If not specified, use 0
			//InitVal = ConstantInt::get(*TheContext, APInt(64, 0));
			InitVal = GetValueFromDataType(&this->dt);
		}

		AllocaInst* Alloca = CreateEntryBlockAlloca(TheFunction, VarName, GetLLVMTypeFromDataType(&this->dt));
		Builder->CreateStore(InitVal, Alloca);

		//OldBindings.push_back(symbolTable.getVariable(VarName));
		OldBindings.push_back(scopeManager.getVariableFromCurrentScope(VarName));

		// Remember this binding.
		//symbolTable.addVariable(VarName, Alloca);
		scopeManager.addVariableToCurrentScope(VarName, Alloca);
	}
	// Codegen the body, now that all vars are in scope.
	if (Body == nullptr)
	{
		return GetValueFromDataType(&this->dt);
	}
	Value* BodyVal = Body->accept(&visitor);
	if (!BodyVal)
		return nullptr;

	// Pop all our variables from scope.
	for (unsigned i = 0, e = VarNames.size(); i != e; ++i)
		//symbolTable.addVariable(VarNames[i].first, OldBindings[i]);
		scopeManager.addVariableToCurrentScope(VarNames[i].first, OldBindings[i]);

	// Return the body computation.
	return BodyVal;
}

Value* StructExprAST::codegen()
{

	return nullptr;
}

Value* ReturnExprAST::codegen()
{
	CodegenVisitor visitor;

	Value* Result = ReturnValue->accept(&visitor);
	if (!Result)
		return nullptr;

	// Emit the return instruction
	Builder->CreateRet(Result);
	return Result;
}

Value* ScopeExprAST::codegen()
{
	CodegenVisitor visitor;

	if (scopeManager.isScopeExisting(Name) || IsPersistent)
	{
		scopeManager.createPersistentScope(Name);
		scopeManager.enterPersistentScope(Name);
	}
	else
	{
		scopeManager.enterTempScope();
	}

	// Get the builder of the scope
	Builder = scopeManager.getBuilderOfCurrentScope();
	ModuleVar = scopeManager.getModuleOfCurrentScope();

	std::unique_ptr<FunctionAST> function = nullptr;
	std::unique_ptr<PrototypeAST> proto = nullptr;

	// Generate the code for each expression in the body
	for (auto& Expr : Body)
	{

		if (FunctionAST* funcAST = dynamic_cast<FunctionAST*>(Expr.get()))
		{
			function.reset(static_cast<FunctionAST*>(funcAST));
			Expr.release();
			if (Function* RetFunc = funcAST->accept(&visitor))
			{
				if (RetFunc == nullptr)
				{
					printf("[CODEGEN-SCOPE] Error while trying to generate Function Code for scope!\n");
				}
				else
				{
					RetFunc->print(errs());
				}
			}
		}
		else if (PrototypeAST* protoAST = dynamic_cast<PrototypeAST*>(Expr.get()))
		{
			proto.reset(static_cast<PrototypeAST*>(protoAST));
			Expr.release();
			if (Function* RetFunc = protoAST->accept(&visitor))
			{
				if (RetFunc == nullptr)
				{
					printf("[CODEGEN-SCOPE] Error while trying to generate Prototype Code for scope!\n");
				}
				else
				{
					RetFunc->print(errs());
				}
			}
		}
		else
		{
			if (Value* RetVal = Expr->accept(&visitor))
			{
				// Check if any error occurred during code generation
				if (RetVal == nullptr)
				{
					printf("[CODEGEN-SCOPE] Error while trying to generate Expression Code for scope!\n");
				}
			}
		}
	}

	// Restore previous scope
	if (scopeManager.isScopeExisting(Name) || IsPersistent)
	{
		scopeManager.exitPersistentScope();
	}
	else
	{
		scopeManager.exitTempScope();
	}

	// Restore Builder
	Builder = scopeManager.getBuilderOfCurrentScope();
	ModuleVar = scopeManager.getModuleOfCurrentScope();

	return nullptr;
	
}

// CreateEntryBlockAlloca - Create an alloca instr in the entry block of the function
// Used for mut. vars etc.
AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type* type)
{
	// Create an IRBuilder obj which points at the first instruction of the entry block
	IRBuilder<> TmpBuilder(
		&TheFunction->getEntryBlock(),
		TheFunction->getEntryBlock().begin());

	// Create an alloca with VarName
	return TmpBuilder.CreateAlloca(type, nullptr, VarName);
}

void InitializeModule(bool optimizations)
{
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

		// Promote allocas to registers.
		TheFPM->add(createPromoteMemoryToRegisterPass());
		// Do simple "peephole" optimizations and bit-twiddling optzns.
		TheFPM->add(createInstructionCombiningPass());
		// Reassociate expressions.
		TheFPM->add(createReassociatePass());

	}

	TheFPM->doInitialization();

	// Create a new builder for the module.

	scopeManager.setContext(TheContext.get());
	Builder = scopeManager.getBuilderOfCurrentScope();
	ModuleVar = scopeManager.getModuleOfCurrentScope();
	//Builder = std::make_unique<IRBuilder<>>(*TheContext);
	//Builder2 = std::make_unique<IRBuilder<>>(*TheContext);
}

void InitializeJIT()
{
	std::string error;
	llvm::raw_string_ostream error_os(error);
	if (llvm::verifyModule(*TheModule, &error_os)) {
		std::cerr << "Module Error: " << error << '\n';
		TheModule->dump();
	}

	llvm::EngineBuilder engineBuilder(std::move(TheModule));

	engineBuilder
		.setErrorStr(&error)
		.setOptLevel(llvm::CodeGenOpt::Aggressive)
		.setEngineKind(llvm::EngineKind::JIT);

	engine = engineBuilder.create();
}

int MergeModulesAndPrint()
{
	
	llvm::Linker linker(*TheModule);

	for (auto& module : scopeManager.getAllModules()) {
		if (linker.linkInModule(std::move(module))) {
			errs() << "Error linking module.\n";
			return 1;
		}
	}
	TheModule->print(errs(), nullptr);

	return 1;
}

int ObjectCodeGen()
{
	auto TargetTriple = sys::getDefaultTargetTriple();
	TheModule->setTargetTriple(TargetTriple);

	std::string Error;
	auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

	// Print an error and exit if we couldn't find the requested target.
	// This generally occurs if we've forgotten to initialise the
	// TargetRegistry or we have a bogus target triple.
	if (!Target) {
		errs() << Error;
		return 1;
	}

	auto CPU = "generic";
	auto Features = "";

	TargetOptions opt;
	auto RM = std::optional<Reloc::Model>();
	auto TheTargetMachine =
		Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

	TheModule->setDataLayout(TheTargetMachine->createDataLayout());

	auto Filename = "output.o";
	std::error_code EC;
	raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

	if (EC) {
		errs() << "Could not open file: " << EC.message();
		return 1;
	}

	legacy::PassManager pass;
	auto FileType = CGFT_ObjectFile;

	if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
		errs() << "TheTargetMachine can't emit a file of this type";
		return 1;
	}

	pass.run(*TheModule);
	dest.flush();

	outs() << "Wrote " << Filename << "\n";

	return 0;
}

void InitializeTargets()
{
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();
	llvm::InitializeAllTargetInfos();
	llvm::InitializeAllTargets();
	llvm::InitializeAllTargetMCs();
	llvm::InitializeAllAsmPrinters();
}

Type* GetLLVMTypeFromDataType(DataType* dt)
{
	switch(*dt)
	{
	case DT_I8:
		return IntegerType::get(*TheContext, 8);
	case DT_I32:
		return IntegerType::get(*TheContext, 32);
	case DT_I64:
		return IntegerType::get(*TheContext, 64);
	case DT_DOUBLE:
		return Type::getDoubleTy(*TheContext);
	case DT_FLOAT:
		return Type::getFloatTy(*TheContext);
	case DT_VOID:
		return Type::getVoidTy(*TheContext);
	case DT_BOOL:
	case DT_CHAR:
	case DT_UNKNOWN:
	default:
		return nullptr;
	}
}

Value* GetValueFromDataType(DataType* dt, double Val)
{
	auto type = GetLLVMTypeFromDataType(dt);

	switch (*dt)
	{
	case DT_I8:
		return ConstantInt::get(*TheContext, APInt(8, Val));
	case DT_I32:
		return ConstantInt::get(*TheContext, APInt(32, Val));
	case DT_I64:
		return ConstantInt::get(*TheContext, APInt(64, Val));
	case DT_DOUBLE:
		return ConstantFP::get(*TheContext, APFloat(Val));
	case DT_FLOAT:
		return ConstantFP::get(*TheContext, APFloat(Val));
	case DT_VOID:
	case DT_BOOL:
	case DT_CHAR:
	case DT_UNKNOWN:
	default:
		return nullptr;
	}

}