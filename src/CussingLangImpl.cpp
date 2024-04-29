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
	//"struct lul {x: i64, y: i64 };"
	
	//"scoped::testo->test(1,2);"
	
	//"persistent scope testo { fn test(x: i32, y: i32) -> i32 { let z: i32 = (x + y) * 2;x = z + 3;return x;} }",
	
	//"persistent scope testo { fn main() -> i32 { return test(1,2); } }",
	
	//"persistent scope testo { fn test(x: i32, y: i32) -> i32 {let z: i32 = (x + y) * 2; x = z + 3; return x; }};",
	
	//"fn test_two_args_multiple_stmt_return(x: i32, y: i32) -> i32 {let z: i32 = (x + y) * 2; x = z + 3; return x; }",
	//"fn test_two_args_multiple_stmt_with_assign_return(x: i32, y: i32) -> i32 {x = (x + y) * 2; x = x + 3; return x; }"
	
	//"fn test_let_and_return_multiple_statements(x: i32) -> i32 { let y: i32 = 2; return x + y; }",
	//"test_let_and_return_multiple_statements(1);"

	//"extern printd(x);",
	//"fn test_extern_with_dp(x) -> i64 { printd(x) : x = 4 : printd(x);"

	//"fn fib(x) -> i64 {if (x < 3) then 1 else fib(x-1) + fib(x-2);"
	
	//"fn unary!(v) { if v then 0 else 1; }",
 
	//"fn test_negation(a) -> i64 { !a; 

	//"fn binary> 10 (LHS RHS) { RHS < LHS; }"

	//"extern putchard(char);",
	//"fn test_extern_print(n) -> i64 { for i = 1, i < n, 1 f in putchard(42); }",
	//"test_extern_print(100)"

	//"extern putchard(char);",
	//"fn foo(a: i32, b: i32) -> i32 { if a then foo(a,b) else putchard(a); }"
	//"fn bar(a: i32) -> i32 { foo(a, 2) + bar(1331); }",
	//"fn test_function_call_with_extern(a: i32) -> i32 { bar(a); }"
	
	//"fn test(x: i32) -> i32 { (1 + 2) + x; }",
	
	//"fn test(x: i32) -> i32 { (1 + 2 + x); (3 + 4 + x); }",
	
	//"fn test_arith_parenthesis(x: i32) -> i32 { (1+2+x)*(x+(1+2)); }"

	//"fn test_addition(x: i32, y: i32) -> i32 {x + y;}",
	//"test_addition(2,3);"
	"fn test(x: i32) {1 + 2;}"
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