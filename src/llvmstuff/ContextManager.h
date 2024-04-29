#ifndef CONTEXTMANAGER_H
#define CONTEXTMANAGER_H

#include <memory>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"

class ContextManager 
{
private:
    static std::shared_ptr<ContextManager> instance;
    std::shared_ptr<llvm::LLVMContext> context;

    ContextManager() : context(std::make_shared<llvm::LLVMContext>()) {}

public:
    static std::shared_ptr<ContextManager> getInstance() 
    {
        if (!instance) 
        {
            instance = std::shared_ptr<ContextManager>(new ContextManager());
        }
        return instance;
    }

    std::shared_ptr<llvm::LLVMContext> getContext() 
    {
        return context;
    }
};
#endif // !CONTEXTMANAGER_H