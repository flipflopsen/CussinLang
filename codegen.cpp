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
	printf("CodegenVisitor is visiting NumberExprAST");

	return ast->codegen();
}
Value *CodegenVisitor::visit(VariableExprAST* ast)
{
	printf("CodegenVisitor is visiting VariableExprAST");

	return ast->codegen();
}
Value *CodegenVisitor::visit(BinaryExprAST* ast)
{
	printf("CodegenVisitor is visiting BinaryExprAST");

	return ast->codegen();
}
Value *CodegenVisitor::visit(CallExprAST* ast)
{
	printf("CodegenVisitor is visiting CallExprAST");

	return ast->codegen();
}
Function *CodegenVisitor::visit(PrototypeAST* ast)
{
	printf("CodegenVisitor is visiting PrototypeAST");

	return ast->codegen();
}
Function *CodegenVisitor::visit(FunctionAST* ast)
{
	printf("CodegenVisitor is visiting FunctionAST");

	return ast->codegen();
}


// Codegen implementations

Value *NumberExprAST::codegen()
{ 
	printf("Performing code generation for NumberExprAST.");
	return ConstantFP::get(*TheContext, APFloat(Val));
}

Value *VariableExprAST::codegen()
{
	printf("Performing code generation for VariableExprAST.");
	Value* V = NamedValues[Name];
	if (!V)
		LogErrorV("Unknown variable name");
	return V;
}

Value *BinaryExprAST::codegen()
{
	CodegenVisitor visitor;

	printf("Performing code generation for BinaryExprAST.");
	Value* L = LHS->accept(&visitor);
	Value* R = RHS->accept(&visitor);
	if (!L || !R)
		return nullptr;

	switch (Op) {
	case '+':
		return BuilderPtr->CreateFAdd(L, R, "addtmp");
	case '-':
		return BuilderPtr->CreateFSub(L, R, "subtmp");
	case '*':
		return BuilderPtr->CreateFMul(L, R, "multmp");
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

	printf("Performing code generation for CallExprAST.");
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
	printf("Performing code generation for PrototypeAST.");
	return nullptr;
}

Function *FunctionAST::codegen()
{
	printf("Performing code generation for FunctionAST.");
	return nullptr;
}
