#include "codegen.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

using namespace llvm;

void LogError(const char* Str) {
	fprintf(stderr, "[CODEGEN-ERROR] %s\n", Str);
}

Value* LogErrorV(const char* Str) {
	LogError(Str);
	return nullptr;
}

static std::unique_ptr<LLVMContext> TheContext;
static IRBuilder<> Builder(*TheContext);
static std::unique_ptr<IRBuilder<>> BuilderPtr(&Builder);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value*> NamedValues;

// Visitor implementations

Value *CodegenVisitor::visit(NumberExprAST* ast)
{
	printf("CodegenVisitor is visiting NumberExprAST\n");
	auto ret = ast->codegen();
	TheModule->print(errs(), nullptr);
	return ret;
}
Value *CodegenVisitor::visit(VariableExprAST* ast)
{
	printf("CodegenVisitor is visiting VariableExprAST\n");

	auto ret = ast->codegen();
	TheModule->print(errs(), nullptr);
	return ret;
}
Value *CodegenVisitor::visit(BinaryExprAST* ast)
{
	printf("CodegenVisitor is visiting BinaryExprAST\n");

	auto ret = ast->codegen();
	TheModule->print(errs(), nullptr);
	return ret;
}
Value *CodegenVisitor::visit(CallExprAST* ast)
{
	printf("CodegenVisitor is visiting CallExprAST\n");

	auto ret = ast->codegen();
	TheModule->print(errs(), nullptr);
	return ret;
}
Function *CodegenVisitor::visit(PrototypeAST* ast)
{
	printf("CodegenVisitor is visiting PrototypeAST\n");

	auto ret = ast->codegen();
	TheModule->print(errs(), nullptr);
	return ret;
}
Function *CodegenVisitor::visit(FunctionAST* ast)
{
	printf("CodegenVisitor is visiting FunctionAST\n");

	auto ret = ast->codegen();
	TheModule->print(errs(), nullptr);
	return ret;
}


// Codegen implementations

Value *NumberExprAST::codegen()
{ 
	printf("Performing code generation for NumberExprAST.\n");
	return ConstantInt::get(*TheContext, APInt(64, Val));
}

Value *VariableExprAST::codegen()
{
	printf("Performing code generation for VariableExprAST.\n");
	Value* V = NamedValues[Name];
	if (!V)
		LogErrorV("Unknown variable name");
	return V;
}

Value *BinaryExprAST::codegen()
{
	CodegenVisitor visitor;

	printf("Performing code generation for BinaryExprAST.\n");
	Value* L = LHS->accept(&visitor);
	Value* R = RHS->accept(&visitor);
	if (!L || !R)
	{
		printf("Returning nullptr\n");
		return nullptr;
	}

	printf("Operator: %c\n", Op);

	switch (Op) {
	case '+':
		return BuilderPtr->CreateAdd(L, R, "addtmp");
	case '-':
		return BuilderPtr->CreateSub(L, R, "subtmp");
	case '*':
		return BuilderPtr->CreateMul(L, R, "multmp");
	case '<':
		L = BuilderPtr->CreateFCmpULT(L, R, "cmptmp");
		// Convert bool 0/1 to double 0.0 or 1.0
		return BuilderPtr->CreateUIToFP(L, Type::getDoubleTy(*TheContext),
			"booltmp");
	default:
		return LogErrorV("invalid binary operator");
	}
}

Value *CallExprAST::codegen()
{
	CodegenVisitor visitor;

	printf("Performing code generation for CallExprAST.\n");
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

	return BuilderPtr->CreateCall(CalleeF, ArgsV, "calltmp");
}

Function *PrototypeAST::codegen()
{
	printf("Performing code generation for PrototypeAST.\n");

	llvm::Type* returnType = llvm::Type::getInt64Ty(*TheContext);

	std::vector<llvm::Type*> paramTypes;
	for (const auto& arg : Args) {
		// Assuming all parameter types are i64 for this example
		paramTypes.push_back(llvm::Type::getInt64Ty(*TheContext));
	}


	// Create the function type
	llvm::FunctionType* functionType = llvm::FunctionType::get(returnType, paramTypes, false);

	// Create the function
	llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, Name, TheModule.get());

	// Set names for the function parameters
	llvm::Function::arg_iterator argIterator = function->arg_begin();
	for (const auto& arg : Args) {
		argIterator->setName(arg);
		++argIterator;
	}

	return function;
}

Function *FunctionAST::codegen()
{
	printf("Performing code generation for FunctionAST.\n");

	CodegenVisitor visitor;

	// Create a new LLVM module
	Function* TheFunction = TheModule->getFunction(Proto->getName());

	if (!TheFunction)
		TheFunction = Proto->accept(&visitor);

	if (!TheFunction)
		return nullptr;

	if (!TheFunction->empty())
		return (Function*)LogErrorV("Function cannot be redefined.");


	// Create a new basic block in the function
	BasicBlock* basicBlock = BasicBlock::Create(*TheContext, "entry", TheFunction);

	BuilderPtr->SetInsertPoint(basicBlock);

	// Record the function arguments in the NamedValues map.
	NamedValues.clear();

	int ctr = 0;
	for (auto& Arg : TheFunction->args())
	{
		NamedValues[std::string(Arg.getName())] = &Arg;
		ctr++;
	}
	printf("Added %d NamedValues.\n", ctr);

	// Generate the code for the body expression
	if (Value* RetVal = Body->accept(&visitor)) {
		// Finish off the function.
		printf("Finishing off function!\n");
		BuilderPtr->CreateRet(RetVal);

		// Validate the generated code, checking for consistency.
		verifyFunction(*TheFunction);

		return TheFunction;
	}

	// TODO: Better error handling
	TheFunction->eraseFromParent();

	return nullptr;
}


void InitializeModule() {
	// Open a new context and module.
	TheContext = std::make_unique<LLVMContext>();
	TheModule = std::make_unique<Module>("my cool jit", *TheContext);

}
