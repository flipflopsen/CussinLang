#include "headers/CodegenVisitor.h"
#include "headers/PrototypeExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Function* PrototypeAST::codegen()
{
	printf("[CODEGEN] Performing code generation for PrototypeAST.\n");

	Type* returnType = GetLLVMTypeFromDataType(&this->returnType);

	std::vector<Type*> paramTypes;
	for (const auto& arg : Args) 
	{
		// Assuming all parameter types are i64 for this example
		paramTypes.push_back(GetLLVMTypeFromDataType(const_cast<DataType*>(&arg.second)));
	}

	// Create the function type
	FunctionType* functionType = FunctionType::get(returnType, paramTypes, false);

	// Create the function
	Function* function = Function::Create(functionType, Function::ExternalLinkage, Name, *ModuleVar);

	// Set names for the function parameters
	llvm::Function::arg_iterator argIterator = function->arg_begin();
	for (const auto& arg : Args) 
	{
		argIterator->setName(arg.first);
		++argIterator;
	}

	ModuleVar->print(errs(), nullptr);

	return function;
}