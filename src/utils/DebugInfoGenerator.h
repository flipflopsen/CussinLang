
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"

class DebugInfoGenerator {
private:
	std::unique_ptr<llvm::DIBuilder> DBuilder;
	llvm::DICompileUnit* CompileUnit;
	llvm::DIFile* File;
	std::vector<llvm::DIScope*> LexicalBlocks;

public:
	/// Initialize the debug information builder
	void InitializeDebugInfo(llvm::Module* M, const std::string& filename,
		const std::string& directory = ".") {
		DBuilder = std::make_unique<llvm::DIBuilder>(*M);

		// Create file descriptor
		File = DBuilder->createFile(filename, directory);

		// Create compile unit
		CompileUnit = DBuilder->createCompileUnit(
			llvm::dwarf::DW_LANG_C_plus_plus_14,    // Language
			File,                              // File
			"Cussin Compiler v1.0",            // Producer string
			false,                             // isOptimized
			"",                                // Flags
			0,                                 // Runtime version
			"",                                // SplitName
			llvm::DICompileUnit::DebugEmissionKind::FullDebug,
			0,                                 // DWOId
			true,                              // SplitDebugInlining
			false                              // DebugInfoForProfiling
		);
	}

	/// Create debug info for a function
	llvm::DISubprogram* CreateFunctionDebugInfo(llvm::Function* F, unsigned LineNo,
		unsigned ScopeLineNo = 0) {
		if (ScopeLineNo == 0) ScopeLineNo = LineNo;

		// Create function return type
		llvm::SmallVector<llvm::Metadata*, 8> EltTys;
		llvm::DIType* RetType = nullptr;

		// Get return type
		llvm::Type* FRetType = F->getReturnType();
		if (FRetType->isVoidTy()) {
			RetType = nullptr;  // void return
		}
		else if (FRetType->isIntegerTy()) {
			unsigned BitWidth = FRetType->getIntegerBitWidth();
			RetType = DBuilder->createBasicType("int", BitWidth,
				llvm::dwarf::DW_ATE_signed);
		}
		else if (FRetType->isFloatingPointTy()) {
			unsigned BitWidth = FRetType->getPrimitiveSizeInBits();
			RetType = DBuilder->createBasicType("float", BitWidth,
				llvm::dwarf::DW_ATE_float);
		}
		else if (FRetType->isPointerTy()) {
			llvm::DIType* PointeeTy = DBuilder->createBasicType("int", 32,
				llvm::dwarf::DW_ATE_signed);
			RetType = DBuilder->createPointerType(PointeeTy,
				64);  // 64-bit pointer
		}

		EltTys.push_back(RetType);

		// Add parameter types
		for (auto& Arg : F->args()) {
			llvm::DIType* ArgType = nullptr;
			llvm::Type* ArgTy = Arg.getType();

			if (ArgTy->isIntegerTy()) {
				unsigned BitWidth = ArgTy->getIntegerBitWidth();
				ArgType = DBuilder->createBasicType("int", BitWidth,
					llvm::dwarf::DW_ATE_signed);
			}
			else if (ArgTy->isFloatingPointTy()) {
				unsigned BitWidth = ArgTy->getPrimitiveSizeInBits();
				ArgType = DBuilder->createBasicType("float", BitWidth,
					llvm::dwarf::DW_ATE_float);
			}
			else if (ArgTy->isPointerTy()) {
				llvm::DIType* PointeeTy = DBuilder->createBasicType("int", 32,
					llvm::dwarf::DW_ATE_signed);
				ArgType = DBuilder->createPointerType(PointeeTy, 64);
			}
			else {
				// Default to int for unknown types
				ArgType = DBuilder->createBasicType("int", 32,
					llvm::dwarf::DW_ATE_signed);
			}

			EltTys.push_back(ArgType);
		}

		// Create function type
		llvm::DISubroutineType* FuncType = DBuilder->createSubroutineType(
			DBuilder->getOrCreateTypeArray(EltTys));

		// Create subprogram (function debug info)
		llvm::DISubprogram* SP = DBuilder->createFunction(
			File,                          // Scope (file)
			F->getName(),                  // Name
			llvm::StringRef(),                   // Linkage name
			File,                          // File
			LineNo,                        // Line number
			FuncType,                      // Type
			ScopeLineNo,                   // Scope line
			llvm::DINode::FlagPrototyped,        // Flags
			llvm::DISubprogram::SPFlagDefinition // Subprogram flags
		);

		F->setSubprogram(SP);
		return SP;
	}

	/// Create debug info for a local variable
	llvm::DILocalVariable* CreateVariableDebugInfo(llvm::DIScope* Scope,
		const std::string& Name,
		unsigned LineNo,
		llvm::DIType* Type,
		unsigned ArgNo = 0) {
		if (ArgNo > 0) {
			// This is a function parameter
			return DBuilder->createParameterVariable(Scope, Name, ArgNo,
				File, LineNo, Type);
		}
		else {
			// This is a local variable
			return DBuilder->createAutoVariable(Scope, Name, File, LineNo, Type);
		}
	}

	/// Create debug info for a basic type
	llvm::DIType* CreateBasicTypeDebugInfo(const std::string& Name,
		uint64_t SizeInBits,
		unsigned Encoding) {
		return DBuilder->createBasicType(Name, SizeInBits, Encoding);
	}

	/// Create pointer type debug info
	llvm::DIType* CreatePointerTypeDebugInfo(llvm::DIType* PointeeTy,
		uint64_t SizeInBits = 64) {
		return DBuilder->createPointerType(PointeeTy, SizeInBits);
	}

	/// Insert a debug location to an instruction
	void InsertDebugLocation(llvm::Instruction* I, unsigned Line, unsigned Col,
		llvm::DIScope* Scope) {
		llvm::DILocation* Loc = llvm::DILocation::get(Scope->getContext(), Line, Col, Scope);
		I->setDebugLoc(llvm::DebugLoc(Loc));
	}

	/// Create a lexical block (for nested scopes like loops, if statements)
	llvm::DILexicalBlock* CreateLexicalBlock(llvm::DIScope* Scope, unsigned Line,
		unsigned Col) {
		llvm::DILexicalBlock* Block = DBuilder->createLexicalBlock(Scope, File,
			Line, Col);
		LexicalBlocks.push_back(Block);
		return Block;
	}

	/// Insert variable declaration debug info
	void InsertDeclare(llvm::AllocaInst* Storage, llvm::DILocalVariable* VarInfo,
		llvm::DIExpression* Expr, const llvm::DILocation* DL,
		llvm::Instruction* InsertBefore) {
		DBuilder->insertDeclare(Storage, VarInfo, Expr, DL, InsertBefore);
	}

	/// Insert variable value debug info
	void InsertDbgValueIntrinsic(llvm::Value* Val, llvm::DILocalVariable* VarInfo,
		llvm::DIExpression* Expr, const llvm::DILocation* DL,
		llvm::Instruction* InsertBefore) {
		DBuilder->insertDbgValueIntrinsic(Val, VarInfo, Expr, DL, InsertBefore);
	}

	/// Create an empty expression (for simple variables)
	llvm::DIExpression* CreateExpression() {
		return DBuilder->createExpression();
	}

	/// Finalize all debug info (must be called before object code generation)
	void Finalize() {
		DBuilder->finalize();
	}

	/// Get the compile unit
	llvm::DICompileUnit* GetCompileUnit() const {
		return CompileUnit;
	}

	/// Get the file
	llvm::DIFile* GetFile() const {
		return File;
	}
};
