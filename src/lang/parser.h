#ifndef PARSER_H
#define PARSER_H

#include <map>

#include "../utils/Datatypes.h"
#include "../utils/BinopPrecedence.h"
#include "lexer.h"
#include "ast/headers/BinaryExpressionAST.h"
#include "ast/headers/CallExpressionAST.h"
#include "ast/headers/ForExpressionAST.h"
#include "ast/headers/FunctionExpressionAST.h"
#include "ast/headers/IfExpressionAST.h"
#include "ast/headers/LetExpressionAST.h"
#include "ast/headers/NumberExpressionAST.h"
#include "ast/headers/PrototypeExpressionAST.h"
#include "ast/headers/ReturnExpressionAST.h"
#include "ast/headers/ScopeExpressionAST.h"
#include "ast/headers/StructExpressionAST.h"
#include "ast/headers/UnaryExpressionAST.h"
#include "ast/headers/VariableExpressionAST.h"

extern std::map<char, int> BinopPrecedence;

class Parser
{
public:
	TokenArray Tokens;
	int CurTok;
	int Count;
	int Position;

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
	std::unique_ptr<ExprAST> ParseIfExpr();
	std::unique_ptr<ExprAST> ParseForExpr();
	std::unique_ptr<ExprAST> ParseLetExpr();
	std::unique_ptr<ExprAST> ParseStructExpr();
	std::unique_ptr<ExprAST> ParseReturnExpr();
	std::unique_ptr<ScopeExprAST> ParseScopeExpr();
	std::unique_ptr<PrototypeAST> ParseExtern();
	std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);
	std::unique_ptr<PrototypeAST> ParsePrototype(bool is_extern);
	std::unique_ptr<FunctionAST> ParseFnDef();
	std::unique_ptr<FunctionAST> ParseTopLevelExpr();
	std::unique_ptr<ExprAST> ParseUnary();
	std::vector<std::unique_ptr<ExprAST>> ParseBlock();

	// Static compiled Handlers
	void HandleExtern();
	void HandleDefinition();
	void HandleTopLevelExpression();
	void HandleScopeExpression();

	// JIT Handlers
	void HandleExternJIT();
	void HandleDefinitionJIT();
	void HandleTopLevelExpressionJIT();

	// Access to token buf
	Token getNextToken();
	Token PeekNextToken();
	Token PeekCurrentToken();
	Token PeekNextNextToken();
	bool IsOperator(int type);

	DataType EvaluateDataTypeOfToken(int tokenPos);
};

#endif