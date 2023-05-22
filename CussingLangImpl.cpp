// CussingLangImpl.cpp : Defines the entry point for the application.
//

#include "CussingLangImpl.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <cstdio>

using namespace std;

int main()
{
	printf("Starting CussingLangImpl \n");

	bool running = true;


	while (running)
	{
		char input[4096];
		GetInput(input);

		if (!input)
			printf("no input string recieved\n");

		TokenArray token_array = LexInput(input);
		DebugPrintTokenArray(token_array);

		auto parser = Parser::Parser(token_array);
		//parser.outputVals();
		parser.Parse();
		//ParseExpression(token_array.tokens, token_array.count, token_dict, ArrayCount(token_dict));
		DeleteTokens(token_array);
	}
	return 0;
}