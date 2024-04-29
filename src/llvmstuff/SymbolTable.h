#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "../lang/ast/headers/PrototypeExpressionAST.h"

using namespace llvm;

struct StructInfo {
    llvm::StructType* structType;
    std::unordered_map<std::string, unsigned> memberIndices;
};

class SymbolTable {
private:
    std::unique_ptr<IRBuilder<>> builder;
    std::unique_ptr<Module> module;
    std::map<std::string, AllocaInst*> variableTable;
    std::map<std::string, std::unique_ptr<PrototypeAST>> functionTable;
    std::unordered_map<std::string, StructInfo> structTable;

public:
    SymbolTable() = default;

    SymbolTable(std::unique_ptr<IRBuilder<>> builder, Module& module)
        : builder(std::move(builder)), module(&module) {}


    void addVariable(const std::string& name, AllocaInst* value) {
        if (value) {
            variableTable[name] = value;
        }
    }

    void removeVariable(const std::string& name) {
        variableTable.erase(name);
    }

    AllocaInst* getVariable(const std::string& name) const {
        auto it = variableTable.find(name);
        return it != variableTable.end() ? it->second : nullptr;
    }

    void addFunction(const std::string& name, std::unique_ptr<PrototypeAST> function) {
        if (function) {
            functionTable[name] = std::move(function);
        }
    }

    PrototypeAST* getFunction(const std::string& name) const {
        auto it = functionTable.find(name);
        return it != functionTable.end() ? it->second.get() : nullptr;
    }

    void addStruct(const std::string& structName, StructType* structType) {
        if (structType) {
            StructInfo structInfo{ structType };
            structTable[structName] = std::move(structInfo);
        }
    }

    StructType* getStruct(const std::string& structName) const {
        auto it = structTable.find(structName);
        return it != structTable.end() ? it->second.structType : nullptr;
    }

    unsigned getMemberIndex(const std::string& structName, const std::string& memberName) const {
        auto it = structTable.find(structName);
        if (it != structTable.end()) {
            const auto& memberIt = it->second.memberIndices.find(memberName);
            if (memberIt != it->second.memberIndices.end()) {
                return memberIt->second;
            }
        }
        return static_cast<unsigned>(-1); // Invalid index
    }

    IRBuilder<>* getBuilder() const {
        return builder.get();
    }

    Module* getModule() const {
        return module.get();
    }
};

#endif  // SYMBOLTABLE_H
