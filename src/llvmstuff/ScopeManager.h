#ifndef SCOPEMANAGER_H
#define SCOPEMANAGER_H

#include <stack>
#include <map>
#include <vector>
#include <string>

#include "SymbolTable.h"
#include "../utils/util.h"

class ScopeManager {
private:
    LLVMContext* TheContext;
    std::vector<SymbolTable> tmpScopeStack;
    std::map<std::string, SymbolTable> persistentScopeMap;
    std::map<std::string, IRBuilder<>> builders;
    // 0 = GlobalScope, n > 0 others
    std::vector<std::string> scopeLevels; // Stack of string to refer to different scopes and builders
    int depth = 0;
    SymbolTable* CurrentScope;
    SymbolTable* superScope;

    ScopeManager() {
    }

    // Private destructor to prevent deletion of the instance
    ~ScopeManager() {
        // Destructor implementation
    }

    // Private copy constructor and copy assignment operator to prevent copying
    ScopeManager(const ScopeManager&) = delete;
    ScopeManager& operator=(const ScopeManager&) = delete;

    // Private move constructor and move assignment operator to prevent moving
    ScopeManager(ScopeManager&&) = delete;
    ScopeManager& operator=(ScopeManager&&) = delete;

public:

    static ScopeManager& getInstance()
	{
        static ScopeManager instance;
        return instance;
    }

    void setContext(LLVMContext* context)
	{
        TheContext = context;
        auto Builder = std::make_unique<IRBuilder<>>(*TheContext);
        persistentScopeMap["GLOBAL"] = SymbolTable(std::move(Builder));
        CurrentScope = &persistentScopeMap["GLOBAL"];
        superScope = CurrentScope;
	}


    void createPersistentScope(std::string name)
    {
	    if (persistentScopeMap.count(name) > 0)
	    {
            fprintf(stderr, "[SCOPE-MANAGER-ERR] Persistent scope with name: %s already exists!\n", name);
            return;
	    }

        if (strcompare(name.data(), "GLOBAL"))
        {
            fprintf(stderr, "[SCOPE-MANAGER-ERR] Cannot create a scope named 'GLOBAL', it's reserved!\n");
            return;
        }

        fprintf(stderr, "[SCOPE-MANAGER] Creating persistent scope with name: %s\n", name);
        auto Builder = std::make_unique<IRBuilder<>>(*TheContext);
        persistentScopeMap[name] = SymbolTable(std::move(Builder));
        scopeLevels[depth] = name;
        depth = scopeLevels.size();
    }

    void enterPersistentScope(const std::string& name)
    {
        superScope = CurrentScope;
        CurrentScope = &persistentScopeMap[name];
    }

    void exitPersistentScope()
    {
        CurrentScope = superScope;
    }

    void enterGlobalScope()
    {
        CurrentScope = &persistentScopeMap["GLOBAL"];
        superScope = CurrentScope;
    }

    void enterTempScope()
	{
        fprintf(stderr, "[SCOPE-MANAGER] Entering temporary scope depth: %d\n", tmpScopeStack.size());
        auto Builder = std::make_unique<IRBuilder<>>(*TheContext);
    	tmpScopeStack.push_back(SymbolTable(std::move(Builder)));
        superScope = CurrentScope;
        CurrentScope = &tmpScopeStack.back();

        scopeLevels.push_back("tmpscope" + std::to_string(depth));
        depth = scopeLevels.size();
    }

    void exitTempScope()
	{
        if (!tmpScopeStack.empty()) {
            tmpScopeStack.pop_back();
            return;
        }
        scopeLevels.pop_back();
        depth = scopeLevels.size();
        CurrentScope = superScope;
    }

    void addVariableToPersistentScope(const std::string& name, AllocaInst* value, const std::string& scope = "GLOBAL")
    {
        addVariableToScope(false, name, value, scope);
    }

    AllocaInst* getVariableFromPersistentScope(const std::string& name, const std::string& scope = "GLOBAL")
    {
        return getVariableFromScope(false, name, scope);
    }

    void addFunctionToPersistentScope(const std::string& name, std::unique_ptr<PrototypeAST> function, const std::string& scope = "GLOBAL")
    {
        addFunctionToScope(false, name, std::move(function), scope);
    }

    PrototypeAST* getFunctionFromPersistentScope(const std::string& name, const std::string& scope = "GLOBAL")
    {
        return getFunctionFromScope(false, name, scope);
    }

    void addStructToPersistentScope(const std::string& structName, llvm::StructType* structType, const std::string& scope = "GLOBAL")
    {
        addStructToScope(false, structName, structType, scope);
    }

    StructType* getStructFromPersistentScope(const std::string& name, const std::string& scope = "GLOBAL")
    {
        return getStructFromScope(false, name, scope);
    }


    void addVariableToCurrentScope(const std::string& name, AllocaInst* value)
	{
        addVariableToScope(true, name, value);
    }

    AllocaInst* getVariableFromCurrentScope(const std::string& name)
	{
        return getVariableFromScope(true, name);
    }

    void addFunctionToCurrentScope(const std::string& name, std::unique_ptr<PrototypeAST> function)
    {
        addFunctionToScope(true, name, std::move(function));
    }

    PrototypeAST* getFunctionFromCurrentScope(const std::string& name)
    {
        return getFunctionFromScope(true, name);
    }

    void addStructToCurrentScope(const std::string& structName, llvm::StructType* structType)
    {
        addStructToScope(true, structName, structType);
    }

    StructType* getStructFromCurrentScope(const std::string& name)
    {
        return getStructFromScope(true, name);
    }

    void addVariableToScope(bool currentScope, const std::string& name, AllocaInst* value, const std::string& scope = "tmp")
    {
        if (currentScope)
            (*CurrentScope).addVariable(name, value);
        else
            persistentScopeMap[scope].addVariable(name, value);
        
    }

    void addFunctionToScope(bool currentScope, const std::string& name, std::unique_ptr<PrototypeAST> function, const std::string& scope = "tmp")
    {
        if (currentScope)
            (*CurrentScope).addFunction(name, std::move(function));
        else
            persistentScopeMap[scope].addFunction(name, std::move(function));
    }

    void addStructToScope(bool currentScope, const std::string& structName, llvm::StructType* structType, const std::string& scope = "tmp")
    {
        if (currentScope)
            (*CurrentScope).addStruct(structName, structType);
        else
			persistentScopeMap[scope].addStruct(structName, structType);
    }

    AllocaInst* getVariableFromScope(bool currentScope, const std::string& name, const std::string& scope = "tmp")
    {
        if (currentScope)
            return (*CurrentScope).getVariable(name);
        return persistentScopeMap[scope].getVariable(name);

    }

    PrototypeAST* getFunctionFromScope(bool currentScope, const std::string& name, const std::string& scope = "tmp")
    {
        if (currentScope)
            return (*CurrentScope).getFunction(name);
        return persistentScopeMap[scope].getFunction(name);
    }

    StructType* getStructFromScope(bool currentScope, const std::string& structName, const std::string& scope = "tmp")
    {
        if (currentScope)
            return (*CurrentScope).getStruct(structName);
    	return persistentScopeMap[scope].getStruct(structName);
    }

    IRBuilder<>* getBuilderOfCurrentScope()
    {
        return (*CurrentScope).getBuilder();
    }
};

#endif