#include "CussingLangImpl.h"
#include "utils/util.h"
#include "lang/lexer.h"
#include "lang/parser.h"
#include "llvmstuff/codegen.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{
  constexpr bool jit = false;
  constexpr bool optimizations = false;

  // Enter interactive mode after all entries in `inputs` have been processed.
  constexpr bool interactiveAfterTests = false;

  // Wait for user confirmation after all compilation output has been produced.
  constexpr bool waitBeforeExit = true;

	// TODO: Migrate these manual test cases to GoogleTest.
	//
	// Uncomment one or more entries to execute them before entering
	// the interactive input loop.
	const std::vector<std::string> inputs =
	{
		// Structs
		// "struct lul {x: i64, y: i64 };",

		// Scopes
		// "scoped::testo->test(1,2);",

		// "persistent scope testo { fn test(x: i32, y: i32) -> i32 { let z: i32 = (x + y) * 2;x = z + 3;return x;} }",
		// "persistent scope testo { fn main() -> i32 { return test(1,2); } }",
		// "persistent scope testo { fn test(x: i32, y: i32) -> i32 {let z: i32 = (x + y) * 2; x = z + 3; return x; }};",

		// Statements, assignments, and returns
		// "fn test_two_args_multiple_stmt_return(x: i32, y: i32) -> i32 {let z: i32 = (x + y) * 2; x = z + 3; return x; }",
		// "fn test_two_args_multiple_stmt_with_assign_return(x: i32, y: i32) -> i32 {x = (x + y) * 2; x = x + 3; return x; }",

		// "fn test_let_and_return_multiple_statements(x: i32) -> i32 { let y: i32 = 2; return x + y; }",
		// "test_let_and_return_multiple_statements(1);",

		// External functions
		// "extern printd(x);",

		// Intentionally malformed/incomplete parser input
		// "fn test_extern_with_dp(x) -> i64 { printd(x) : x = 4 : printd(x);",

		// Recursion
		// "fn fib(x: i32) -> i32 {if (x < 3) then 1 else fib(x - x) + fib(x - 2) };",
		// "fn fib(x: i32) -> i32 {if (x < 3) then 1 else fib(x - 1) + fib(x - 2);};",
		// "fib(10);",

		// Unary and binary operators
		// "fn unary!(v) { if v then 0 else 1; }",

		// Intentionally malformed/incomplete parser input
		// "fn test_negation(a) -> i64 { !a;",

		// "fn binary> 10 (LHS RHS) { RHS < LHS; }",

		// Loops
		// "extern putchard(char);",
		// "fn test_extern_print(n: i32) -> i32 { for i: i32 = 1, i < n, 1 fin 1; };",
		// "test_extern_print(100);",

		// Output
		// This declaration is repeated intentionally because it belongs
		// to this independently selectable test group.
		// "extern putchard(char);",

		// Intentionally malformed/incomplete parser input
		// "fn printstar(n) -> i64 { for i = 1, i < n, 1 fin putchard(42);",

		// "printstar(100);",

		// Function calls
		// "fn foo(a: i64, b: i64) -> i64 { if a then foo(a,b) else putchard(a); }",
		// "fn bar(a) -> i64 { foo(a, 2) + bar(1331); }",

		// Arithmetic using i64
		// "fn test(x: i64) -> i64 { (1 + 2) + x; }",
		// "fn test(x: i64) -> i64 { x = (1 + 2 + x); return (3 + 4 + x); }",
		// "fn test2(x: i64) -> i64 { return ((1+2+x)*(x+(1+2))); }",

		// Strings
		// "fn test(x: str) -> str {x = 'lul123'; return x;}",

		// Function calls using i32
		// "fn foo(a: i32, b: i32) -> i32 { if a then foo(a,b) else putchard(a); }",
		// "fn bar(a: i32) -> i32 { foo(a, 2) + bar(1331); }",
		// "fn test_function_call_with_extern(a: i32) -> i32 { bar(a); }",

		// Arithmetic using i32
		// "fn test(x: i32) -> i32 { (1 + 2) + x; }",
		// "fn test(x: i32) -> i32 { (1 + 2 + x); (3 + 4 + x); }",
		// "fn test_arith_parenthesis(x: i32) -> i32 { (1+2+x)*(x+(1+2)); }",

		// Addition
		// "fn test_addition(x: i32, y: i32) -> i32 {x + y;}",
		// "test_addition(2,3);",

		// Untyped return
		// "fn test(x: i32) {1 + 2;}",
	};
}

void MainLoop()
{
	std::size_t inputIndex = 0;

	while (inputIndex < inputs.size() || interactiveAfterTests)
	{
		char input[8192] = {};

		if (inputIndex < inputs.size())
		{
			const std::string& testInput = inputs[inputIndex];

			if (testInput.size() >= sizeof(input))
			{
				std::fprintf(
					stderr,
					"Test input %zu exceeds the input buffer size.\n",
					inputIndex
				);

				++inputIndex;
				continue;
			}

			std::memcpy(
				input,
				testInput.data(),
				testInput.size()
			);

			input[testInput.size()] = '\0';

			std::fprintf(
				stderr,
				"test[%zu]> %s\n",
				inputIndex,
				input
			);

			++inputIndex;
		}
		else
		{
			std::fprintf(stderr, "ready> ");
			GetInput(input);
		}

		if (input[0] == '\0')
		{
			std::fprintf(stderr, "No input string received.\n");

			if (interactiveAfterTests)
				continue;

			break;
		}

		if (std::strcmp(input, "stop") == 0)
			break;

		const TokenArray tokenArray = LexInput(input);
		DebugPrintTokenArray(tokenArray);

		Parser parser(tokenArray);
		parser.Parse(jit);

		ObjectCodeGen();

		// DeleteTokens(tokenArray);
	}

	MergeModulesAndPrint();
	std::printf("Printed!\n");

	CompileWithDebugInfo();
	std::printf("Code generated!\n");
}

void WaitBeforeExit()
{
	if (!waitBeforeExit)
		return;

	std::printf("\nExecution finished. Press Enter to exit...");
	std::fflush(stdout);

	std::string ignored;
	std::getline(std::cin, ignored);
}

int main()
{
	std::printf("Starting CussingLangImpl\n");

	InitializeTargets();
	InitializeModule(optimizations);

	// InitializeJIT();

	MainLoop();
	WaitBeforeExit();

	return 0;
}