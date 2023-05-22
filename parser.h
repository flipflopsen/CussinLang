#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

enum DataType
{
	DT_BOOL,
	DT_CHAR,
	DT_SHORT_INT,
	DT_UNSIGNED_SHORT_INT,
	DT_INT,
	DT_UNSIGNED_INT,
	DT_DOUBLE,
	DT_FLOAT,

	/* Pointers */
	DT_POINTER_TO_INT,
	DT_POINTER_TO_DOUBLE,
	DT_POINTER_TO_CHAR,
	DT_POINTER_TO_STRUCT,
	DT_POINTER_TO_FLOAT,
	DT_OTHER
};

struct TokenDictionaryItem
{
	char const * key;
	void* value;
	char* datatype;
};

class Parser
{
public:
	TokenArray Tokens;
	int CurTok;
	int Count;
	Parser(TokenArray tokens);

	static std::unique_ptr<ExprAST> ParseExpression();
	void outputVals();
private:
	std::unique_ptr<ExprAST> LogError(const char* Str);
	std::unique_ptr<PrototypeAST> LogErrorP(const char* Str);
	Token getNextToken();
};

#endif