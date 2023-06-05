#ifndef CODEGEN_H
#define CODEGEN_H

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

#include "../utils/Datatypes.h"
#include "../utils/logger.h"

extern void InitializeModule(bool optimizations);
extern void InitializeJIT();
extern int ObjectCodeGen();
extern void InitializeTargets();
extern int MergeModulesAndPrint();

extern llvm::Value* GetNumValueFromDataType(DataType* dt, double Val = 0);
extern llvm::Type* GetLLVMTypeFromDataType(DataType* dt);
extern llvm::Function* getFunction(std::string Name);
extern llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function* TheFunction, const std::string& VarName, llvm::Type* type);

#endif