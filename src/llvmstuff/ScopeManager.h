#ifndef SCOPEMANAGER_H
#define SCOPEMANAGER_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <mutex>
#include "SymbolTable.h"
#include "ModuleManager.h"
#include "ContextManager.h"
#include "../utils/util.h"

class ScopeManager {
private:
    static std::once_flag _initFlag;
    static std::unique_ptr<ScopeManager> _instance;

    std::map<std::string, SymbolTable> _persistentScopes;
    std::vector<std::unique_ptr<Module>> _modules;
    SymbolTable* _currentScope = nullptr;
    SymbolTable* _superScope = nullptr;

    ScopeManager() = default; // Private constructor for singleton

public:
    ScopeManager(const ScopeManager&) = delete;
    ScopeManager& operator=(const ScopeManager&) = delete;
    ScopeManager(ScopeManager&&) = delete;
    ScopeManager& operator=(ScopeManager&&) = delete;

    ~ScopeManager() = default; // Use default if no special cleanup needed

    static ScopeManager& getInstance() {
        std::call_once(_initFlag, []() {
            _instance.reset(new ScopeManager);
            });
        return *_instance;
    }

    void initializeGlobalScope() {
        createPersistentScope("GLOBAL");  // Initialize global scope
        enterPersistentScope("GLOBAL");
    }

    bool createPersistentScope(const std::string& name);
    void enterPersistentScope(const std::string& name);
    void exitPersistentScope();
    void enterGlobalScope();
    void enterTempScope();
    void exitTempScope();
    void setContext();
    IRBuilder<>* getBuilderOfCurrentScope();
    Module* getModuleOfCurrentScope();
    std::vector<std::unique_ptr<Module>> getAllModules();
    bool isScopeExisting(const std::string& name);


    void addVariable(bool currentScope, const std::string& name, AllocaInst* value, const std::string& scope = "GLOBAL");
    AllocaInst* getVariable(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL");
    void removeVariable(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL");

    void addFunction(bool currentScope, const std::string& name, std::unique_ptr<PrototypeAST> function, const std::string& scope = "GLOBAL");
    PrototypeAST* getFunction(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL");
    void removeFunction(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL");

    void addStruct(bool currentScope, const std::string& name, StructType* type, const std::string& scope = "GLOBAL");
    StructType* getStruct(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL");
    void removeStruct(bool currentScope, const std::string& name, const std::string& scope = "GLOBAL");

private:
    void handleScopeCreationError(const std::string& name, const std::string& message);
};

#endif // SCOPEMANAGER_H