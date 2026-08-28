#ifndef DATASTORAGE_H
#define DATASTORAGE_H

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/LegacyPassManager.h>

#include "ScopeManager.h"

extern std::shared_ptr<llvm::LLVMContext> TheContext;
extern std::shared_ptr<llvm::legacy::FunctionPassManager> TheFPM;
extern llvm::ExecutionEngine* engine;
extern std::unique_ptr<llvm::Module> TheModule;
extern llvm::Module* ModuleVar;
extern llvm::IRBuilder<>* Builder;

#endif