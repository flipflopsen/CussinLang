#include "ScopeManager.h"

std::once_flag ScopeManager::_initFlag;
std::unique_ptr<ScopeManager> ScopeManager::_instance;
std::shared_ptr<ContextManager> ContextManager::instance = nullptr;
std::shared_ptr<ModuleManager> ModuleManager::instance = nullptr;

void ScopeManager::setContext()
{
    // Correctly using std::make_unique to avoid copying and ensure proper use of move semantics
    auto contextManager = ContextManager::getInstance();
    auto context = contextManager->getContext();
    auto builder = std::make_unique<IRBuilder<>>(*context);
    auto moduleManager = ModuleManager::getInstance(context);
    auto& module = moduleManager->getModule("cussinJIT-GLOBAL");

    // Check if 'GLOBAL' scope already exists and handle accordingly
    auto it = _persistentScopes.find("GLOBAL");
    if (it != _persistentScopes.end()) {
        // Optionally handle re-initialization or just skip reassignment
        std::cerr << "Warning: 'GLOBAL' scope is already initialized and will not be reinitialized." << std::endl;
        return; // Early return to avoid overwriting existing 'GLOBAL' scope
    }

    // Use emplace to construct the SymbolTable directly in the map to avoid copying
    _persistentScopes.emplace(std::make_pair("GLOBAL", SymbolTable(std::move(builder), std::move(*module))));
    _currentScope = &_persistentScopes["GLOBAL"];
    _superScope = _currentScope;
}


IRBuilder<>* ScopeManager::getBuilderOfCurrentScope()
{
    return (*_currentScope).getBuilder();
}

Module* ScopeManager::getModuleOfCurrentScope()
{
    return (*_currentScope).getModule();
    //return nullptr;
}

std::vector<std::unique_ptr<Module>> ScopeManager::getAllModules()
{
    std::vector<std::unique_ptr<Module>> allModules;

    for (auto& entry : _persistentScopes) {
        SymbolTable& symbolTable = entry.second;
        Module* module = symbolTable.getModule();

        if (module != nullptr) {
            allModules.push_back(std::unique_ptr<Module>(symbolTable.getModule()));
        }
    }
    return allModules;
}

bool ScopeManager::isScopeExisting(const std::string& name)
{
    if (_persistentScopes.count(name) > 0)
        return true;
    return false;
}

// Scope Management
bool ScopeManager::createPersistentScope(const std::string& name)
{
    if (_persistentScopes.count(name) > 0)
    {
        handleScopeCreationError(name, "Persistent scope already exists!");
        return false;
    }

    if (name == "GLOBAL" && _persistentScopes.count(name) > 0)
    {
        handleScopeCreationError(name, "Cannot create a scope named 'GLOBAL', it's reserved!");
        return false;
    }

    auto contextManager = ContextManager::getInstance();
    auto context = contextManager->getContext();

    auto Builder = std::make_unique<IRBuilder<>>(*context);
    //auto TheModule = std::make_unique<Module>("cussinJIT-" + name, *context);
    auto moduleManager = ModuleManager::getInstance(context);
    auto& module = moduleManager->getModule("cussinJIT-" + name);
    _persistentScopes.emplace(std::make_pair(name, SymbolTable(std::move(Builder), std::move(*module))));
    return true;
}

void ScopeManager::enterPersistentScope(const std::string& name)
{
    if (_persistentScopes.count(name) == 0)
    {
        handleScopeCreationError(name, "Attempting to enter a non-existent persistent scope");
        return;
    }
    _superScope = _currentScope;
    _currentScope = &_persistentScopes[name];
}

void ScopeManager::exitPersistentScope()
{
    if (!_superScope)
    {
        std::cerr << "[SCOPE-MANAGER] No parent scope to return to." << std::endl;
        return;
    }
    _currentScope = _superScope;
}

void ScopeManager::enterGlobalScope()
{
    if (_persistentScopes.count("GLOBAL") == 0)
    {
        std::cerr << "[SCOPE-MANAGER] Global scope is not initialized." << std::endl;
        return;
    }
    _currentScope = &_persistentScopes["GLOBAL"];
    _superScope = _currentScope;
}

void ScopeManager::enterTempScope()
{
    _tempScopeDepth++;
    auto contextManager = ContextManager::getInstance();
    auto context = contextManager->getContext();

    std::string identifier = "tmpscope-" + std::to_string(_modules.size());
    auto Builder = std::make_unique<IRBuilder<>>(*context);
    auto moduleManager = ModuleManager::getInstance(context);
    auto& module = moduleManager->getModule("cussinJIT-" + identifier);
    //auto TheModule = std::make_unique<Module>("cussinJIT-" + identifier, *context);
    SymbolTable tempScope(std::move(Builder), std::move(*module));
    //_modules.push_back(std::make_unique<Module>(*TheModule));  // Storing unique_ptr to the Module
    _superScope = _currentScope;
    _currentScope = &tempScope;
}

void ScopeManager::exitTempScope()
{
    if (_modules.empty())
    {
        std::cerr << "[SCOPE-MANAGER] No temporary scope to exit." << std::endl;
        return;
    }
    _modules.pop_back();  // Remove the last module, effectively destroying the temporary scope
    _tempScopeDepth--;
    _currentScope = _superScope;
}

// Variable and Function Management
void ScopeManager::addVariable(bool currentScope, const std::string& name, AllocaInst* value, const std::string& scope)
{
    if (currentScope)
    {
        _currentScope->addVariable(name, value);
        return;
    }

    if (_persistentScopes.count(scope) == 0)
    {
        handleScopeCreationError(scope, "Attempting to add variable to a non-existent scope");
        return;
    }
    _persistentScopes[scope].addVariable(name, value);
}

AllocaInst* ScopeManager::getVariable(bool currentScope, const std::string& name, const std::string& scope)
{
    if (currentScope)
        return _currentScope->getVariable(name);

    if (_persistentScopes.count(scope) == 0)
    {
        handleScopeCreationError(scope, "Attempting to get variable from a non-existent scope");
        return nullptr;
    }
    return _persistentScopes[scope].getVariable(name);
}

void ScopeManager::removeVariable(bool currentScope, const std::string& name, const std::string& scope)
{
    if (currentScope)
    {
        _currentScope->removeVariable(name);
        return;
    }

    if (_persistentScopes.count(scope) == 0)
        handleScopeCreationError(scope, "Attempting to remove a variable from a non-existent scope");

    _persistentScopes[scope].removeVariable(name);
}

void ScopeManager::addFunction(bool currentScope, const std::string& name, std::unique_ptr<PrototypeAST> function, const std::string& scope)
{
    if (currentScope)
    {
        _currentScope->addFunction(name, std::move(function));
        return;
    }

    if (_persistentScopes.count(scope) == 0)
    {
        handleScopeCreationError(scope, "Attempting to add function to a non-existent scope");
        return;
    }
    _persistentScopes[scope].addFunction(name, std::move(function));
}

PrototypeAST* ScopeManager::getFunction(bool currentScope, const std::string& name, const std::string& scope)
{
    if (currentScope)
        return _currentScope->getFunction(name);

    if (_persistentScopes.count(scope) == 0)
    {
        handleScopeCreationError(scope, "Attempting to get function from a non-existent scope");
        return nullptr;
    }
    return _persistentScopes[scope].getFunction(name);
}

void ScopeManager::removeFunction(bool currentScope, const std::string& name, const std::string& scope)
{
    /*
    if (currentScope)
        _currentScope->removeFunction(name);

    if (_persistentScopes.count(scope) == 0)
        handleScopeCreationError(scope, "Attempting to remove a function from a non-existent scope");

    _persistentScopes[scope].removeFunction(name);
    */
}

void ScopeManager::addStruct(bool currentScope, const std::string& name, StructType* type, const std::string& scope)
{
    if (currentScope)
    {
        _currentScope->addStruct(name, type);
        return;
    }

    if (_persistentScopes.count(scope) == 0)
    {
        handleScopeCreationError(scope, "Attempting to add struct to a non-existent scope");
        return;
    }
    _persistentScopes[scope].addStruct(name, type);
}

StructType* ScopeManager::getStruct(bool currentScope, const std::string& name, const std::string& scope)
{
    if (currentScope)
    {
        return _currentScope->getStruct(name);
    }
    if (_persistentScopes.count(scope) == 0)
    {
        handleScopeCreationError(scope, "Attempting to get struct from a non-existent scope");
        return nullptr;
    }
    return _persistentScopes[scope].getStruct(name);
}

void ScopeManager::removeStruct(bool currentScope, const std::string& name, const std::string& scope)
{
    /*
    if (currentScope)
        _currentScope->removeStruct(name);

    if (_persistentScopes.count(scope) == 0)
        handleScopeCreationError(scope, "Attempting to remove a struct from a non-existent scope");

    _persistentScopes[scope].removeStruct(name);
    */
}

int ScopeManager::getTempScopeDepth()
{
    return _tempScopeDepth;
}

// Error handling
void ScopeManager::handleScopeCreationError(const std::string& name, const std::string& message) {
    std::cerr << "[SCOPE-MANAGER-ERR] " << message << " Scope name: " << name << std::endl;
}