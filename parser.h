#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <map>

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
	int Position;
	std::map<char, int> BinopPrecedence;

	Parser(TokenArray tokens);

	void Parse();

	// Debug
	void outputVals();
private:
	// Binary operations
	int GetTokenPrecedence();

	// Parsers
	std::unique_ptr<ExprAST> ParseExpression();
	std::unique_ptr<ExprAST> ParsePrimary();
	std::unique_ptr<ExprAST> ParseNumberExpr();
	std::unique_ptr<ExprAST> ParseIdentifierExpr();
	std::unique_ptr<ExprAST> ParseParenExpr();
	std::unique_ptr<ExprAST> ParseBraceExpr();
	std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);
	std::unique_ptr<PrototypeAST> ParsePrototype();
	std::unique_ptr<FunctionAST> ParseFnDef();
	std::unique_ptr<FunctionAST> ParseTopLevelExpr();

	// Handlers
	void HandleDefinition();
	void HandleTopLevelExpression();


	// Access to token buf
	Token getNextToken();
	bool IsOperator(int type);
};

#endif