#ifndef PARSER_H
#define PARSER_H

#include "../utils/Datatypes.h"
#include "lexer.h"
#include <map>

#include "ast.h"

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
	std::unique_ptr<ExprAST> ParseLetExpr();
	std::unique_ptr<PrototypeAST> ParseExtern();
	std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);
	std::unique_ptr<PrototypeAST> ParsePrototype(bool is_extern);
	std::unique_ptr<FunctionAST> ParseFnDef();
	std::unique_ptr<FunctionAST> ParseTopLevelExpr();
	std::unique_ptr<ExprAST> ParseUnary();

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
	Token PeekNextNextToken();
	bool IsOperator(int type);

	DataType EvaluateDataTypeOfToken(int tokenPos);
};

#endif