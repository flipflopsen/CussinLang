#include "CussingLangImpl.h"
#include "utils/util.h"
#include "lang/lexer.h"
#include "lang/parser.h"
#include "llvmstuff/codegen.h"
#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;

constexpr bool jit = false;
constexpr bool optimizations = false;

//TODO: Migrate this to GTest
const std::vector<std::string> inputs =
{
	//"scoped::testo->test(1,2);"
	//"persistent scope testo { fn test(x: i32, y: i32) -> i32 { let z: i32 = (x + y) * 2;x = z + 3;return x;} }",
	//"persistent scope testo { fn main() -> i32 { return test(1,2); } }",
	//"fn main() -> i32 { return test(1,2); }"
	"fn test(y: i32) -> i32 { let z: i32 = 2; return y + z; }"
	//"persistent scope testo { fn test(x: i32, y: i32) -> i32 {let z: i32 = (x + y) * 2; x = z + 3; return x; }};",
	//"scope testo {test(1,2); };",
	//"test(1,2); "
	//"fn test(x: i32, y: i32) -> i32 {let z: i32 = (x + y) * 2; x = z + 3; return x; }",
	//"fn test2(x: i32, y: i32) -> i32 {x = (x + y) * 2; x = x + 3; return x; }"
	//"fn doubleit(x: i32, y: i32) -> i32 { (x + y) * 2 ; }"
	//"fn binary : 1 (x y) { y; }",
	//"fn test(x: i32) -> i32 { let y: i32 = 2; return x + y; }",
	//"test(1);"
	//"extern printd(x);",
	//"fn test(x) -> i64 { printd(x) : x = 4 : printd(x);"
	//"fn fib(x) -> i64 {if (x < 3) then 1 else fib(x-1) + fib(x-2);"
	//"fn unary!(v) { if v then 0 else 1; }",
	//"fn test(a) -> i64 { !a; }"
	//"fn binary> 10 (LHS RHS) { RHS < LHS; }"
	//"extern putchard(char);",
	//"fn printstar(n) -> i64 { for i = 1, i < n, 1 fin putchard(42);",
	//"printstar(100)"
	//"fn foo(a, b) -> i64 { if a then foo(a,b) else putchard(a); }"
	//"fn bar(a) -> i64 { foo(a, 2) + bar(1331); }",
	//"fn test(x) -> i64 { (1 + 2) + x; }",
	//"fn test(x) -> i64 { (1 + 2 + x); (3 + 4 + x); }",
	//"fn test2(x) -> i64 { (1+2+x)*(x+(1+2)); }"
	//"fn test(x) -> i64 {4 + 5;}"
};

void MainLoop()
{
	int ctr = 0;

	while (true)
	{
		fprintf(stderr, "ready> ");
		char input[8192];

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

		//ObjectCodeGen();

		ctr++;
		//DeleteTokens(token_array);
	}

	MergeModulesAndPrint();
	printf("Printed!\n");
	ObjectCodeGen();
	printf("Code generated!\n");
}


int main()
{
	printf("Starting CussingLangImpl \n");


	InitializeTargets();

	InitializeModule(optimizations);

	//InitializeJIT();

	MainLoop();
}