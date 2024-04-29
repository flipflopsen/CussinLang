#include "headers/CodegenVisitor.h"
#include "headers/LetExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* LetExprAST::codegen()
{
	auto& scopeManager = ScopeManager::getInstance();

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
			InitVal = GetNumValueFromDataType(&this->dt);
		}

		AllocaInst* Alloca = CreateEntryBlockAlloca(TheFunction, VarName, GetLLVMTypeFromDataType(&this->dt));
		Builder->CreateStore(InitVal, Alloca);

		//OldBindings.push_back(symbolTable.getVariable(VarName));
		OldBindings.push_back(scopeManager.getVariable(true, VarName));

		// Remember this binding.
		//symbolTable.addVariable(VarName, Alloca);
		scopeManager.addVariable(true, VarName, Alloca);
	}
	// Codegen the body, now that all vars are in scope.
	if (Body == nullptr)
	{
		return GetNumValueFromDataType(&this->dt);
	}
	Value* BodyVal = Body->accept(&visitor);
	if (!BodyVal)
		return nullptr;

	// Pop all our variables from scope.
	for (unsigned i = 0, e = VarNames.size(); i != e; ++i)
		scopeManager.addVariable(true, VarNames[i].first, OldBindings[i]);

	// Return the body computation.
	return BodyVal;
}