#include "headers/CodegenVisitor.h"
#include "headers/ScopeExpressionAST.h"
#include "headers/FunctionExpressionAST.h"
#include "headers/PrototypeExpressionAST.h"
#include "../../llvmstuff/codegen.h"

Value* ScopeExprAST::codegen()
{
	auto& scopeManager = ScopeManager::getInstance();

	printf("[CODEGEN] Performing code generation for Scope"
		"AST.\n");
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