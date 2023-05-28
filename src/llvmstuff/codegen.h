#ifndef CODEGEN_H
#define CODEGEN_H

#include "../lang/ast.h"
#include "../utils/Datatypes.h"


void InitializeModule(bool optimizations);
void InitializeJIT();
int ObjectCodeGen();
void InitializeTargets();
Value* GetValueFromDataType(DataType* dt, double Val = 0);
Type* GetLLVMTypeFromDataType(DataType* dt);

#endif