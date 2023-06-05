#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "../lang/ast/headers/PrototypeExpressionAST.h"

#include <map>
#include <string>

struct StructInfo {
    llvm::StructType* structType;
    std::unordered_map<std::string, unsigned> memberIndices;
};

//static std::unique_ptr<IRBuilder<>> Builder;
//static std::map<std::string, std::unique_ptr<PrototypeAST>> functionTable;
//static std::map<std::string, AllocaInst*> variableTable;
//static std::unordered_map<std::string, StructInfo> structTable;

class SymbolTable {
private:
    std::unique_ptr<IRBuilder<>> Builder;
    std::unique_ptr<Module> Module;
    std::map<std::string, AllocaInst*> variableTable;
    std::map<std::string, std::unique_ptr<PrototypeAST>> functionTable;
    std::unordered_map<std::string, StructInfo> structTable;

public:
    SymbolTable()
    {
        for (auto& entry : functionTable) {
            entry.second = std::move(entry.second);
        }
    }

    SymbolTable(std::unique_ptr<IRBuilder<>> Builder, std::unique_ptr<llvm::Module> Module)
	    :Builder(std::move(Builder)), Module(std::move(Module))
    {
        for (auto& entry : functionTable) {
            entry.second = std::move(entry.second);
        }
    }
    void addVariable(const std::string& name, AllocaInst* value)
	{
        variableTable[name] = value;
    }

    void removeVariable(const std::string& name)
    {
        variableTable.erase(name);
    }

    AllocaInst* getVariable(const std::string& name)
	{
        if (variableTable.count(name) > 0)
            return variableTable[name];
        return nullptr;
    }

    void addFunction(const std::string& name, std::unique_ptr<PrototypeAST> function)
	{
        functionTable[name] = std::move(function);
    }

    PrototypeAST* getFunction(const std::string& name)
	{
        auto it = functionTable.find(name);
        if (it != functionTable.end())
            return it->second.get();
        return nullptr;
    }

    void addStruct(const std::string& structName, llvm::StructType* structType)
	{
        StructInfo structInfo;
        structInfo.structType = structType;

        // Initialize member indices
        const auto& memberNames = structType->getStructName();
        for (unsigned i = 0; i < memberNames.size(); ++i) {
            const std::string& memberName = memberNames.str();
            structInfo.memberIndices[memberName] = i;
        }

        structTable[structName] = structInfo;
    }
    

    llvm::StructType* getStruct(const std::string& structName)
	{
        auto it = structTable.find(structName);
        if (it != structTable.end()) {
            return it->second.structType;
        }
        return nullptr;
    }

    unsigned getMemberIndex(const std::string& structName, const std::string& memberName)
	{
        auto it = structTable.find(structName);
        if (it != structTable.end()) {
            const StructInfo& structInfo = it->second;
            auto memberIt = structInfo.memberIndices.find(memberName);
            if (memberIt != structInfo.memberIndices.end()) {
                return memberIt->second;
            }
        }
        // Return an invalid index if the struct or member is not found
        return static_cast<unsigned>(-1);
    }

    IRBuilder<>* getBuilder()
    {
        return Builder.get();
    }

    llvm::Module* getModule()
    {
        return Module.get();
        //return nullptr;
    }
};

#endif