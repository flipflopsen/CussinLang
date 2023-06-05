#ifndef LOGGER_H
#define LOGGER_H


#include "../lang/ast/headers/ExpressionAST.h"
#include "../lang/ast/headers/PrototypeExpressionAST.h"


void LogError(const char* loc, const char* Str);

Value* LogErrorV(const char* Str);

std::unique_ptr<ExprAST> LogError(const char* Str);

std::unique_ptr<PrototypeAST> LogErrorP(const char* Str);

#endif