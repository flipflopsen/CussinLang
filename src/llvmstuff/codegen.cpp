#include "datastorage.h"
#include "codegen.h"
#include "../lang/ast/headers/CodegenVisitor.h"
#include "ContextManager.h"
#include "ModuleManager.h"

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
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
//#include "llvm/ExecutionEngine/MCJIT.h"
#include <llvm/Linker/Linker.h>

#include <codecvt>
#include <iostream>

using namespace llvm;
using namespace llvm::sys;

std::shared_ptr<LLVMContext> TheContext;
std::shared_ptr<legacy::FunctionPassManager> TheFPM;
std::unique_ptr<Module> TheModule;

ExecutionEngine* engine;
Module* ModuleVar;
IRBuilder<>* Builder;


Function* getFunction(std::string Name) {
	auto& scopeManager = ScopeManager::getInstance();
	CodegenVisitor visitor;

	//f (auto* F = ModuleVar->getFunction(Name))
	//    return F;

	PrototypeAST* proto = scopeManager.getFunction(true, Name);
	if (proto)
		return proto->accept(&visitor);

	// If no existing prototype exists, return null.
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
	auto contextManager = ContextManager::getInstance();
	auto context = contextManager->getContext();
	TheContext = context;

	auto moduleManager = ModuleManager::getInstance(context);
	auto& module = moduleManager->getModule("cussinJIT");

	auto moduleCopy = llvm::CloneModule(*module);
	TheModule = std::move(moduleCopy);

	TheFPM = std::make_unique<legacy::FunctionPassManager>(TheModule.get());

	// Open a new context and module.
	//TheContext = std::make_unique<LLVMContext>();
	//TheModule = std::make_unique<Module>("cussinJIT", *TheContext);
	//TheModule->setDataLayout(TheJIT->getDataLayout());
	//TheFPM = std::make_unique<legacy::FunctionPassManager>(TheModule.get());

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
	ScopeManager& manager = ScopeManager::getInstance();
	manager.initializeGlobalScope();
	manager.setContext();
	manager.enterGlobalScope();
	Builder = manager.getBuilderOfCurrentScope();
	ModuleVar = manager.getModuleOfCurrentScope();
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

	auto contextManager = ContextManager::getInstance();
	auto context = contextManager->getContext();

	auto moduleManager = ModuleManager::getInstance(context);
	auto& module = moduleManager->getModule("cussinJIT");

	auto moduleCopy = llvm::CloneModule(*module);

	llvm::EngineBuilder engineBuilder(std::move(moduleCopy));

	engineBuilder
		.setErrorStr(&error)
		.setOptLevel(llvm::CodeGenOpt::Aggressive)
		.setEngineKind(llvm::EngineKind::JIT);

	engine = engineBuilder.create();
}

int MergeModulesAndPrint()
{
	auto contextManager = ContextManager::getInstance();
	auto context = contextManager->getContext();

	auto moduleManager = ModuleManager::getInstance(context);
	auto& module = moduleManager->getModule("cussinJIT");

	llvm::Linker linker(*module);

	// Link other modules as needed
	// Example: linker.linkInModule(otherModule.get());

	module->print(errs(), nullptr);

	return 0;
	/*
	llvm::Linker linker(*TheModule);

	for (auto& module : scopeManager.getAllModules()) {
		if (linker.linkInModule(std::move(module))) {
			errs() << "Error linking module.\n";
			return 1;
		}
	}
	TheModule->print(errs(), nullptr);

	return 0;
	*/
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

Value* GetNumValueFromDataType(DataType* dt, double Val)
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