// CussingLangImpl.cpp : Defines the entry point for the application.
//

#include "CussingLangImpl.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;

int main()
{
	printf("Starting CussingLangImpl \n");

	bool running = true;

	InitializeModule();

	const std::vector<std::string> inputs =
	{
		"fn foo(a, b) -> i64 { a + ( 2 * b ); }",
		"fn bar(a) -> i64 { (foo(a, 2) + bar(1331)); }"
	};

	int ctr = 0;
	while (running)
	{
		fprintf(stderr, "ready> ");
		char input[4096];
		if (ctr < inputs.size())
			strcpy(input, inputs[ctr].c_str());
		else
			GetInput(input);

		if (!input)
			printf("no input string received\n");

		TokenArray token_array = LexInput(input);
		DebugPrintTokenArray(token_array);

		auto parser = Parser::Parser(token_array);
		//parser.outputVals();
		parser.Parse();

		ctr++;

		//ParseExpression(token_array.tokens, token_array.count, token_dict, ArrayCount(token_dict));
		//DeleteTokens(token_array);
	}
	return 0;
}