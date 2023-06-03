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
    // 0 = GlobalScope, n > 0 others
    std::vector<std::string> scopeLevels; // Stack of string to refer to different scopes and builders
    std::vector<std::unique_ptr<Module>> modules;
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
        auto TheModule = std::make_unique<Module>("cussinJIT-GLOBAL", *TheContext);
        persistentScopeMap["GLOBAL"] = SymbolTable(std::move(Builder), std::move(TheModule));
        CurrentScope = &persistentScopeMap["GLOBAL"];
        superScope = CurrentScope;
	}


    void createPersistentScope(std::string name)
    {
	    if (persistentScopeMap.count(name) > 0)
	    {
            fprintf(stderr, "[SCOPE-MANAGER-ERR] Persistent scope with name: %s already exists!\n", name.c_str());
            return;
	    }

        if (strcompare(name.data(), "GLOBAL"))
        {
            fprintf(stderr, "[SCOPE-MANAGER-ERR] Cannot create a scope named 'GLOBAL', it's reserved!\n");
            return;
        }

        fprintf(stderr, "[SCOPE-MANAGER] Creating persistent scope with name: %s\n", name.c_str());
        auto Builder = std::make_unique<IRBuilder<>>(*TheContext);
        auto TheModule = std::make_unique<Module>("cussinJIT-" + name, *TheContext);
    	persistentScopeMap[name] = SymbolTable(std::move(Builder), std::move(TheModule));
        scopeLevels.push_back(name);
        depth = scopeLevels.size();
    }

    void enterPersistentScope(const std::string& name)
    {
        fprintf(stderr, "[SCOPE-MANAGER] Entering persistent scope: %s\n", name.c_str());
        superScope = CurrentScope;
        CurrentScope = &persistentScopeMap[name];
    }

    void exitPersistentScope()
    {
        fprintf(stderr, "[SCOPE-MANAGER] Exiting persistent scope\n");
        CurrentScope = superScope;
    }

    void enterGlobalScope()
    {
        fprintf(stderr, "[SCOPE-MANAGER] Entering global scope\n");
        CurrentScope = &persistentScopeMap["GLOBAL"];
        superScope = CurrentScope;
    }

    void enterTempScope()
	{
        fprintf(stderr, "[SCOPE-MANAGER] Entering temporary scope depth: %d\n", tmpScopeStack.size());
        auto identifier = "tmpscope-" + std::to_string(depth);
    	auto Builder = std::make_unique<IRBuilder<>>(*TheContext);
        auto TheModule = std::make_unique<Module>("cussinJIT-" + identifier, *TheContext);
    	tmpScopeStack.push_back(SymbolTable(std::move(Builder), std::move(TheModule)));
        superScope = CurrentScope;
        CurrentScope = &tmpScopeStack.back();

        scopeLevels.push_back(identifier);
        depth = scopeLevels.size();
    }

    void exitTempScope()
	{
        fprintf(stderr, "[SCOPE-MANAGER] Exiting temp scope\n");
        if (!tmpScopeStack.empty()) {
            tmpScopeStack.pop_back();
            scopeLevels.pop_back();
            depth = scopeLevels.size();
            CurrentScope = superScope;
        }
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

    void removeVariableFromCurrentScope(const std::string& name)
    {
        removeVariableFromScope(true, name);
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

    void addVariableToScope(bool currentScope, const std::string& name, AllocaInst* value, const std::string& scope = "GLOBAL")
    {
        if (currentScope)
            (*CurrentScope).addVariable(name, value);
        else
            persistentScopeMap[scope].addVariable(name, value);
        
    }

    void addFunctionToScope(bool currentScope, const std::string& name, std::unique_ptr<PrototypeAST> function, const std::string& scope = "GLOBAL")
    {
        if (currentScope)
            (*CurrentScope).addFunction(name, std::move(function));
        else
            persistentScopeMap[scope].addFunction(name, std::move(function));
    }

    void addStructToScope(bool currentScope, const std::string& structName, llvm::StructType* structType, const std::string& scope = "GLOBAL")
    {
        if (currentScope)
            (*CurrentScope).addStruct(structName, structType);
        else
			persistentScopeMap[scope].addStruct(structName, structType);
    }

    void removeVariableFromScope(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL")
    {
        if (currentScope)
            (*CurrentScope).removeVariable(name);
        else
            persistentScopeMap[scope].removeVariable(name);
    }
    void removeFunctionFromScope(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL")
    {

    }
    void removeStructFromScope(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL")
    {

    }

    AllocaInst* getVariableFromScope(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL")
    {
        if (currentScope)
            return (*CurrentScope).getVariable(name);
        return persistentScopeMap[scope].getVariable(name);

    }

    PrototypeAST* getFunctionFromScope(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL")
    {
        if (currentScope)
            return (*CurrentScope).getFunction(name);
        return persistentScopeMap[scope].getFunction(name);
    }

    StructType* getStructFromScope(bool currentScope, const std::string& structName, const std::string& scope = "GLOBAL")
    {
        if (currentScope)
            return (*CurrentScope).getStruct(structName);
    	return persistentScopeMap[scope].getStruct(structName);
    }

    IRBuilder<>* getBuilderOfCurrentScope()
    {
        return (*CurrentScope).getBuilder();
    }
    Module* getModuleOfCurrentScope()
    {
        return (*CurrentScope).getModule();
        //return nullptr;
    }
    std::vector<std::unique_ptr<Module>> getAllModules()
    {
        std::vector<std::unique_ptr<Module>> allModules;

        for (auto& entry : persistentScopeMap) {
            SymbolTable& symbolTable = entry.second;
            Module* module = symbolTable.getModule();

            if (module != nullptr) {
                allModules.push_back(std::move(std::unique_ptr<Module>(module)));
            }
        }
        return allModules;
    }

    bool isScopeExisting(const std::string& name)
    {
        if (persistentScopeMap.count(name) > 0)
            return true;
        return false;
    }
};

#endif