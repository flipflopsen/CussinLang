#include "parser.h"

#include "lexer.h"
#include "../utils/util.h"
#include <map>
#include <stack>

std::unique_ptr<ExprAST> LogError(const char* Str);
std::unique_ptr<PrototypeAST> LogErrorP(const char* Str);


Parser::Parser(TokenArray tokens)
{
	Tokens = tokens;
	CurTok = 0;
	Position = 0;
	Count = tokens.count;

	BinopPrecedence['<'] = 10;
	BinopPrecedence['+'] = 20;
	BinopPrecedence['-'] = 20;
	BinopPrecedence['*'] = 40;
}

void Parser::Parse(bool jit)
{
	if (jit)
	{
		while (Position < Count) {
			getNextToken();
			switch (CurTok) {
			case TokenType_EOF:
				return;
			case TokenType_SEMICOLON:
				printf("Got semicolon !\n");
				getNextToken();
				break;
			case TokenType_FN:
				HandleDefinitionJIT();
				break;
			case TokenType_EXTERN:
				HandleExternJIT();
				break;
			default:
				HandleTopLevelExpressionJIT();
				break;
			}
		}
	}
	else
	{
		while (Position < Count) {
			getNextToken();
			switch (CurTok) {
			case TokenType_EOF:
				return;
			case TokenType_SEMICOLON:
				printf("Got semicolon !\n");
				getNextToken();
				break;
			case TokenType_FN:
				HandleDefinition();
				break;
			case TokenType_EXTERN:
				HandleExtern();
				break;
			default:
				HandleTopLevelExpression();
				break;
			}
		}
	}
}

std::unique_ptr<ExprAST> Parser::ParseExpression() {
	auto LHS = ParsePrimary();
	if (!LHS)
		return nullptr;

	return ParseBinOpRHS(0, std::move(LHS));

}


std::unique_ptr<ExprAST> Parser::ParsePrimary()
{
	if (CurTok == TokenType_IDENTIFIER && Tokens.tokens[Position].type == TokenType_LPAREN)
		return ParseCallExpr();
	
	switch (CurTok) {
	case TokenType_SEMICOLON:
		getNextToken(); // Consume the semicolon
		printf("[PARSER] Parsing of Expression is done!\n");
		return ParseExpression();
	case TokenType_IDENTIFIER:
		printf("[PARSER] Parsing Identifier Expression\n");
		return ParseIdentifierExpr();
	case TokenType_DIGIT:
		printf("[PARSER] Parsing number Expression\n");
		return ParseNumberExpr();
	case TokenType_RBRACE:
		printf("[PARSER] Parsing of Body is done!\n");
		return nullptr;
	case TokenType_LPAREN:
		printf("[PARSER] Parsing Paren Expression\n");
		return ParseParenExpr();
	case TokenType_EOF:
		printf("[PARSER] Parsing of File is done!\n");
		return nullptr;
	case TokenType_COMMA:
		printf("[PARSER] Got comma in ParsePrimary!\n");
		return nullptr;
	default:
		printf("[PARSER] Token type of current token: %d\n", CurTok);
		return LogError("Unknown token when expecting an expression");
	};
}

// Parsers

std::unique_ptr<ExprAST> Parser::ParseNumberExpr()
{
	auto token = Tokens.tokens[Position - 1].contents;
	fprintf(stderr, "[PARSER-NR] Parsing number expression for token: %s\n", token);
	auto Result = std::make_unique<NumberExprAST>(strtoint(token));
	return std::move(Result);
}

std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr()
{
	std::string IdName = Tokens.tokens[Position - 1].contents;

	auto lookahead = Tokens.tokens[Position];

	if (lookahead.type == TokenType_COMMA || lookahead.type == TokenType_RPAREN)
	{
		return std::make_unique<VariableExprAST>(IdName);
	}

	// This is vor single vars
	if ((CurTok != TokenType_SEMICOLON && 
		(IsOperator(lookahead.type) || lookahead.type == TokenType_SEMICOLON) &&
		lookahead.type != TokenType_LPAREN) || lookahead.type == TokenType_RPAREN)
		return std::make_unique<VariableExprAST>(IdName);
	
	// If we're here, it's a function call
	fprintf(stderr, "[IMPORTANT] Calling ParseCallExpr from ParseIdentExpr!!");
	return ParseCallArgsExpr();
}

std::unique_ptr<ExprAST> Parser::ParseCallExpr()
{
	std::string IdName = Tokens.tokens[Position - 1].contents;

	getNextToken();

	std::vector<std::unique_ptr<ExprAST>> Args;

	if (CurTok != TokenType_RPAREN)
	{
		while (true)
		{
			if (CurTok != TokenType_COMMA)
			{
				if (auto Arg = ParseCallArgsExpr())
				{
					printf("[PARSER-IDENT] Parsed token with val: %d\n", CurTok);
					Args.push_back(std::move(Arg));
				}
				else
				{
					if (CurTok == TokenType_RPAREN)
					{
						printf("[PARSER-IDENT] Encountered ')' \n");
						break;
					}

					if (CurTok != TokenType_LPAREN)
					{
						printf("[PARSER-IDENT-ERROR] Failed to parse call for tok %d\n", CurTok);
						return nullptr;
					}
					printf("[PARSER-IDENT-ERROR] Encountered ( in CallParsing\n");

				}
			}


			getNextToken();
		}
	}

	return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

std::unique_ptr<ExprAST> Parser::ParseCallArgsExpr()
{
	std::string IdName = Tokens.tokens[Position - 1].contents;

	auto lookahead = Tokens.tokens[Position];

	if (lookahead.type == TokenType_COMMA || lookahead.type == TokenType_RPAREN)
	{
		if (CurTok == TokenType_IDENTIFIER)
			return std::make_unique<VariableExprAST>(IdName);
		return ParseNumberExpr();
	}

	if (IsOperator(lookahead.type))
	{
		return ParseExpression();
	}

	LogErrorP("Returning nullptr in CallArgsExpr, lul?\n");

	return nullptr;
}

std::unique_ptr<ExprAST> Parser::ParseParenExpr()
{
	getNextToken();  // Consume '('

	auto expr = ParseExpression();
	if (!expr)
		return nullptr;

	getNextToken();

	if (CurTok != TokenType_RPAREN)
		return LogError("Expected ')'");


	//getNextToken();  // Consume ')'
	return expr;
}

std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS)
{
	if (ExprPrec < 1)
		getNextToken();

	while (true) {
		int TokPrec = GetTokenPrecedence();

		if (TokPrec < ExprPrec)
			return LHS;

		int BinOp = static_cast<int>(*Tokens.tokens[Position - 1].contents);
		getNextToken(); 

		auto RHS = ParsePrimary();
		if (!RHS)
			return nullptr;

		if (IsOperator(Tokens.tokens[Position].type))
		{

			getNextToken();
			int NextPrec = GetTokenPrecedence();
			if (TokPrec <= NextPrec) {
				RHS = ParseBinOpRHS(TokPrec, std::move(RHS));
				if (!RHS)
					return nullptr;
			}
			else
			{
				RHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
				RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
			}
		}

		// Merge LHS/RHS.
		LHS =
			std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));

		return LHS;
	}
}


std::unique_ptr<PrototypeAST> Parser::ParsePrototype()
{
	printf("[PARSER] Parsing prototype\n");
	if (CurTok != TokenType_IDENTIFIER)
		return LogErrorP("Expected function name in prototype");

	std::string FnName = Tokens.tokens[Position - 1].contents;
	printf("[PARSER] FnName for prototype: %s\n", FnName.c_str());
	std::string ReturnType;

	getNextToken();

	if (CurTok != TokenType_LPAREN)
		return LogErrorP("Expected '(' in prototype");

	std::vector<std::string> ArgNames;
	Token nextToken = getNextToken();
	while (nextToken.type != TokenType_RPAREN)
	{
		if (nextToken.type != TokenType_COMMA)
			ArgNames.push_back(nextToken.contents);
		nextToken = getNextToken();
	}

	if (CurTok != TokenType_RPAREN)
		return LogErrorP("Expected ')' in prototype");

	// success.
	getNextToken(); // eat ')'.

	if (CurTok != TokenType_RETSTMT)
		return LogErrorP("Expected '->' in prototype");

	Token tok = getNextToken(); // eat '->'

	if (CurTok != TokenType_I32 && CurTok != TokenType_I64)
		return LogErrorP("Expected valid return type (i32/i64) in prototype");
	ReturnType = tok.contents;

	getNextToken(); // eat up the return type;

	return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

std::unique_ptr<FunctionAST> Parser::ParseFnDef()
{
	getNextToken(); // eat next token
	auto Proto = ParsePrototype();
	if (!Proto)
		return nullptr;

	if (CurTok != TokenType_LBRACE)
		LogError("Expected '{' for function def");
	fprintf(stderr, "[PARSER] Starting to parse body for %s\n", Tokens.tokens[1].contents);

	getNextToken(); // eat {

	if (auto E = ParseExpression())
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
	return nullptr;
}

std::unique_ptr<FunctionAST> Parser::ParseTopLevelExpr()
{
	if (auto E = ParseExpression()) {
		// Make an anonymous proto.
		auto Proto = std::make_unique<PrototypeAST>("__anon_expr",
			std::vector<std::string>());
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
	}
	return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::ParseExtern()
{
	getNextToken(); // eat extern.
	return ParsePrototype();
}


// Handlers

void Parser::HandleDefinition()
{

	CodegenVisitor visitor;

	printf("[PARSER-Init] Parsing Definition\n");

	if (auto FnAST = ParseFnDef()) {
		fprintf(stderr, "[PARSER-DONE] Got FnAST\n");
		if (auto* FnIR = FnAST->accept(&visitor)) {
			fprintf(stderr, "[PARSER-CG-DONE] Read function definition: \n");
			FnIR->print(errs());
			fprintf(stderr, "\n");
		}
	}
	else {
		// Skip token for error recovery.
		getNextToken();
	}
}

void Parser::HandleTopLevelExpression()
{

	CodegenVisitor visitor;

	printf("[PARSER-Init] Parsing TLE.\n");

	if (auto FnAST = ParseTopLevelExpr()) {
		fprintf(stderr, "[PARSER-DONE] Got TLE-AST\n");
		if (auto* FnIR = FnAST->accept(&visitor)) {
			fprintf(stderr, "[PARSER-CG-DONE] Read top-level expression: \n");
			FnIR->print(errs());
			fprintf(stderr, "\n");

			// Remove the anonymous expression.
			FnIR->eraseFromParent();
		}
	}
	else {
		// Skip token for error recovery.
		getNextToken();
	}
}

void Parser::HandleExtern() {
	if (auto ProtoAST = ParseExtern()) {
		if (auto* FnIR = ProtoAST->codegen()) {
			fprintf(stderr, "Read extern: ");
			FnIR->print(errs());
			fprintf(stderr, "\n");
		}
	}
	else {
		// Skip token for error recovery.
		getNextToken();
	}
}


void Parser::HandleExternJIT()
{
	
}
void Parser::HandleDefinitionJIT()
{
	
}
void Parser::HandleTopLevelExpressionJIT()
{
	
}


void DebugAST()
{
	
}


// Getter for Tokens buf

Token Parser::getNextToken()
{
	Token tok = Parser::Tokens.tokens[Position];
	CurTok = tok.type;
	if (Position < Count)
	{
		Position++;
	}
	else
	{
		//Todo: delete token array and signal done.
	}
	fprintf(stderr, "[PARSER] Getting token nr. %d val: %s, type: %d \n", Position, tok.contents, tok.type);
	return tok;
}

int Parser::GetTokenPrecedence()
{
	char* tokenVal = Tokens.tokens[Position - 1].contents;
	if (!isascii(*tokenVal))
		return -1;

	int TokPrec = BinopPrecedence[*tokenVal];
	if (TokPrec <= 0)
		return -1;
	return TokPrec;
}


// Debug
void Parser::outputVals()
{
	int i = 0;
	while (Position < Count - 1)
	{
		Token tok = getNextToken();
		fprintf(stderr, "[PARSER] Token %d val: %s, type: %d \n", i, tok.contents, tok.type);
		i++;
	}
}


std::unique_ptr<ExprAST> LogError(const char* Str) {
	fprintf(stderr, "[PARSER-ERROR] %s\n", Str);
	return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char* Str) {
	LogError(Str);
	return nullptr;
}


bool Parser::IsOperator(int type)
{
	switch (type)
	{
	case TokenType_PLUS:
	case TokenType_MINUS:
	case TokenType_TIMES:
	case TokenType_SLASH:
	case TokenType_PERIOD:
	case TokenType_EQL:
	case TokenType_NOT:
	case TokenType_LSS:
	case TokenType_GRT:
	case TokenType_LEQ:
	case TokenType_GEQ:
	case TokenType_MOD:
		return true;
	default:
		return false;
	}
}