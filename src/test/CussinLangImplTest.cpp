#include <string>
#include <gtest/gtest.h>
#include "../CussingLangImpl.h"
#include "../utils/util.h"
#include "../lang/lexer.h"
#include "../lang/parser.h"
#include "../llvmstuff/codegen.h"

// Define a test fixture class if needed
class CussingLangImplTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitializeTargets();

        InitializeModule(false);
    }

    void TearDown() override {
        // Perform any necessary cleanup steps after the tests
    }
};


TEST(CussingLangImplTest, TestMethodDeclaration)
{
    char input[4096] = "fn test(x: i32, y: i32) -> i32 { return x + y; }";

    const TokenArray token_array = LexInput(input);
    auto parser = Parser(token_array);
    parser.Parse(false);
    EXPECT_EQ(0, MergeModulesAndPrint());
    EXPECT_EQ(0, ObjectCodeGen());
}

TEST(CussingLangImplTest, TestFunctionCall)
{
    const std::vector<std::string> inputs =
    {
        "fn test(x: i32) -> i32 { let y: i32 = 2; return x + y; }",
        "test(1);"
    };

    for (auto in : inputs)
    {
        char input[8192];
        strcpy(input, in.c_str());
	    const TokenArray token_array = LexInput(input);
	    auto parser = Parser(token_array);
	    parser.Parse(false);
    }
    EXPECT_EQ(0, MergeModulesAndPrint());
    EXPECT_EQ(0, ObjectCodeGen());
}

TEST(CussingLangImplTest, TestArithmetic)
{
    char input[4096] = "fn test() -> i32 { return 1 + 2; }";

    const TokenArray token_array = LexInput(input);
    auto parser = Parser(token_array);
    parser.Parse(false);
    EXPECT_EQ(0, MergeModulesAndPrint());
    EXPECT_EQ(0, ObjectCodeGen());
}

TEST(CussingLangImplTest, TestExtern)
{
    const std::vector<std::string> inputs =
    {
        "extern putchard(char);",
		"fn printstar(n) -> i32 { putchard(42);",
    };

    for (auto in : inputs)
    {
        char input[8192];
        strcpy(input, in.c_str());
        const TokenArray token_array = LexInput(input);
        auto parser = Parser(token_array);
        parser.Parse(false);
    }
    EXPECT_EQ(0, MergeModulesAndPrint());
    EXPECT_EQ(0, ObjectCodeGen());
}

TEST(CussingLangImplTest, TestForLoop)
{
    const std::vector<std::string> inputs =
    {
        "extern putchard(char);",
		"fn printstar(n) -> i64 { for i = 1, i < n, 1 fin putchard(42);",
    };

    for (auto in : inputs)
    {
        char input[8192];
        strcpy(input, in.c_str());
        const TokenArray token_array = LexInput(input);
        auto parser = Parser(token_array);
        parser.Parse(false);
    }
    EXPECT_EQ(0, MergeModulesAndPrint());
    EXPECT_EQ(0, ObjectCodeGen());
}

TEST(CussingLangImplTest, TestControlFlow)
{
    char input[4096] = "fn fib(x: i32) -> i32 {if (x < 3) then 1 else fib(x-1) + fib(x-2);";
    const TokenArray token_array = LexInput(input);
    auto parser = Parser(token_array);
    parser.Parse(false);
    EXPECT_EQ(0, MergeModulesAndPrint());
    EXPECT_EQ(0, ObjectCodeGen());
}

