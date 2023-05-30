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
        // Perform any necessary setup steps for the tests
    }

    void TearDown() override {
        // Perform any necessary cleanup steps after the tests
    }
};

/*
TEST(CussingLangImplTest, Test1)
{
    //const std::string input = "fn test(x: i32, y: i32) -> i32 { let z: i32 = (x + y) * 2; x = z + 3; return x; }";
    // Add assertions or checks here to verify the expected behavior
    // of the code for the given input.

    // Example assertions:
    //EXPECT_EQ(5, 2 + 3);
    //EXPECT_TRUE(true);
}

TEST(CussingLangImplTest, Test2)
{
    // Define another test case here.
}

*/