// CussingLangImpl.cpp : Defines the entry point for the application.
//

#include "CussingLangImpl.h"
#include "utils/util.h"
#include "jit/CussinJIT.h"
#include "lang/lexer.h"
#include "lang/parser.h"
#include "llvmstuff/codegen.h"
#include <iostream>
#include <cstdio>
#include <vector>
#include <llvm/Support/TargetSelect.h>

using namespace std;

constexpr bool jit = false;

const std::vector<std::string> inputs =
{
	//"fn foo(a, b) -> i64 { a + ( 2 * b ); }",
	//"fn bar(a) -> i64 { foo(a, 2) + bar(1331); }",
	//"fn test(x) -> i64 { (1 + 2) + x; }",
	//"fn test(x) -> i64 { (1 + 2 + x); (3 + 4 + x); }",
	"fn test2(x) -> i64 { (1+2+x)*(x+(1+2)); }"
	//"fn test(x) -> i64 {4 + 5;}"
};


int main()
{
	printf("Starting CussingLangImpl \n");


	InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetAsmParser();

	InitializeJIT();

	InitializeModule();

	int ctr = 0;

	while (true)
	{
		fprintf(stderr, "ready> ");
		char input[4096];

		if (ctr < inputs.size())
			strcpy(input, inputs[ctr].c_str());
		else
			GetInput(input);

		if (!input)
			printf("no input string received\n");

		if (std::string("stop").compare(input) == 0)
			break;

		const TokenArray token_array = LexInput(input);
		DebugPrintTokenArray(token_array);

		auto parser = Parser(token_array);
		parser.Parse(jit);

		ctr++;
		//DeleteTokens(token_array);
	}
	return 0;
}