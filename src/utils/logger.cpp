#include "logger.h"

void LogError(const char* loc, const char* Str) {
	fprintf(stderr, "[%s-ERROR] %s\n", Str);
}

Value* LogErrorV(const char* Str) {
	LogError("CODEGEN-VAL", Str);
	return nullptr;
}

std::unique_ptr<ExprAST> LogError(const char* Str) {
	fprintf(stderr, "[PARSER-ERROR] %s\n", Str);
	return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char* Str) {
	LogError(Str);
	return nullptr;
}