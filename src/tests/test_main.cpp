#include <gtest/gtest.h>

// Optional: sample test to verify setup
TEST(Basic, FailingTest) {
    EXPECT_EQ(2 + 2, 5);
}

// Main entry point for all tests
int wmain(int argc, wchar_t** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
