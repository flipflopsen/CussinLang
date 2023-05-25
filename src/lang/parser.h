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
	static std::map<char, int> BinopPrecedence;

	Parser(TokenArray tokens)
	{
		Tokens = tokens;
		CurTok = 0;
		Position = 0;
		Count = tokens.count;
	}

	void Parse(bool jit);

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
	std::unique_ptr<ExprAST> ParseCallExpr();
	std::unique_ptr<ExprAST> ParseCallArgsExpr();
	std::unique_ptr<ExprAST> ParseBraceExpr();
	std::unique_ptr<ExprAST> ParseIfExpr();
	std::unique_ptr<ExprAST> ParseForExpr();
	std::unique_ptr<PrototypeAST> ParseExtern();
	std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);
	std::unique_ptr<PrototypeAST> ParsePrototype(bool is_extern);
	std::unique_ptr<FunctionAST> ParseFnDef();
	std::unique_ptr<FunctionAST> ParseTopLevelExpr();

	// Static compiled Handlers
	void HandleExtern();
	void HandleDefinition();
	void HandleTopLevelExpression();

	// JIT Handlers
	void HandleExternJIT();
	void HandleDefinitionJIT();
	void HandleTopLevelExpressionJIT();

	// Access to token buf
	Token getNextToken();
	Token PeekNextToken();
	Token PeekCurrentToken();
	bool IsOperator(int type);
};

#endif