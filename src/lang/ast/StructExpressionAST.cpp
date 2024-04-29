#include "headers/CodegenVisitor.h"
#include "headers/StructExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* StructExprAST::codegen()
{
	auto& scopeManager = ScopeManager::getInstance();

	llvm::StructType* structType = llvm::StructType::create(*TheContext, getTypeName());

	std::vector<llvm::Type*> elementTypes;

	for (const auto& field : Fields)
	{
		auto dt = field.second;
		elementTypes.push_back(GetLLVMTypeFromDataType(&dt));
	}

	structType->setBody(elementTypes);

	llvm::AllocaInst* structAlloc = Builder->CreateAlloca(structType, nullptr, "struct_" + getTypeName() + "_alloc");

	int ctr = 0;
	for (const auto& field : Fields)
	{
		auto dt = field.second;
		auto name = field.first;

		llvm::Value* memberPtr = Builder->CreateStructGEP(structType, structAlloc, ctr, "struct_" + getTypeName() + "_memberptr_field_" + name);

		Builder->CreateStore(GetNumValueFromDataType(&dt, 0), memberPtr);

		ctr++;
	}

	scopeManager.addStruct(true, getTypeName(), structType);

	TheModule->print(errs(), nullptr);

	return structAlloc;
}