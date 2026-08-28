#include <gtest/gtest.h>
#include "../src/CussingLangImpl.h"
#include "../src/utils/util.h"
#include "../src/lang/lexer.h"
#include "../src/lang/parser.h"
#include "../src/llvmstuff/codegen.h"

class CussinLangImplTest : public ::testing::Test 
{
protected:
    void SetUp() override {
        InitializeTargets();
        InitializeModule(false);
    }

    void TearDown() override {
        // cleanup
    }
};

int ExecuteCode(char input[]) {
    const TokenArray token_array = LexInput(input);
    auto parser = Parser(token_array);
    parser.Parse(false);
    return 0;
}

/*
TEST_F(CussingLangImplTest, TestExternPutchard) {
    char input[] = R"(
        extern putchard(char);
        fn test_extern_print(n: i32) -> i32 { for i = 1, i < n, 1 f in putchard(42); }
        test_extern_print(2);
    )";

    EXPECT_EQ(ExecuteCode(input), 0);
    EXPECT_EQ(MergeModulesAndPrint(), 0);
    EXPECT_EQ(ObjectCodeGen(), 0);
}

TEST_F(CussingLangImplTest, TestMethodDeclaration)
{
    char input[] = "fn test(x: i32, y: i32) -> i32 { return x + y; }";

    EXPECT_EQ(ExecuteCode(input), 0);
    EXPECT_EQ(MergeModulesAndPrint(), 0);
    EXPECT_EQ(ObjectCodeGen(), 0);
}

TEST_F(CussingLangImplTest, TestFunctionCall)
{
    char input[] = R"(
        "fn test(x: i32) -> i32 { let y: i32 = 2; return x + y; }",
        "test(1);"
    )";

    EXPECT_EQ(ExecuteCode(input), 0);
    EXPECT_EQ(MergeModulesAndPrint(), 0);
    EXPECT_EQ(ObjectCodeGen(), 0);
}
*/

TEST_F(CussinLangImplTest, TestArithmetic)
{
    char input[] = R"(fn test() -> i32 { return 1 + 2; };)";

    EXPECT_EQ(ExecuteCode(input), 0);
    EXPECT_EQ(MergeModulesAndPrint(), 0);
    EXPECT_EQ(ObjectCodeGen(), 0);
}

TEST_F(CussinLangImplTest, TestIfElse)
{
    char input[] = R"(fn fib(x: i32) -> i32 {if (x < 3) then 1 else 2;};)";

    EXPECT_EQ(ExecuteCode(input), 0);
    EXPECT_EQ(MergeModulesAndPrint(), 0);
    EXPECT_EQ(ObjectCodeGen(), 0);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}