#include "parser.h"

#include "lexer.h"
#include "util.h"
#include "stdio.h"


Parser::Parser(TokenArray tokens)
{
	Tokens = tokens;
	CurTok = 0;
	Count = tokens.count;
}

Token Parser::getNextToken()
{
	Token tok = Parser::Tokens.tokens[CurTok];
	Parser::CurTok++;
	return tok;
}
std::unique_ptr<ExprAST> Parser::LogError(const char* Str) {
	fprintf(stderr, "Error: %s\n", Str);
	return nullptr;
}
std::unique_ptr<PrototypeAST> Parser::LogErrorP(const char* Str) {
	LogError(Str);
	return nullptr;
}

void Parser::outputVals()
{
	int i = 0;
	while (CurTok < Count - 1)
	{
		Token tok = getNextToken();
		printf("[Parser] Token %d val: %s, type: %s \n", i, tok.contents, TokenTypeToString(tok.type));
		i++;
	}

}