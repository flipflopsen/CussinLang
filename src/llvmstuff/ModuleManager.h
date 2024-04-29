#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <memory>
#include <unordered_map>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

class ModuleManager {
private:
    static std::shared_ptr<ModuleManager> instance;
    std::unordered_map<std::string, std::shared_ptr<llvm::Module>> modules;
    std::shared_ptr<llvm::LLVMContext> context;

    ModuleManager(std::shared_ptr<llvm::LLVMContext> ctx) : context(ctx) {}

public:
    static std::shared_ptr<ModuleManager> getInstance(std::shared_ptr<llvm::LLVMContext> ctx) {
        if (!instance) {
            instance = std::shared_ptr<ModuleManager>(new ModuleManager(ctx));
        }
        return instance;
    }

    std::shared_ptr<llvm::Module> getModule(const std::string& name) {
        auto it = modules.find(name);
        if (it != modules.end()) {
            return it->second;
        }

        // Create a new module if it does not exist
        auto module = std::make_shared<llvm::Module>(name, *context);
        modules[name] = module;
        return module;
    }
};

#endif // MODULEMANAGER_H
