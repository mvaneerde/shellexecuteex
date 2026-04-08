#include <gtest/gtest.h>
#include "common.h"

TEST(Basic, TrivialTest) {
    EXPECT_EQ(2 + 2, 4);
}

int wmain(int argc, LPCWSTR argv[]) {
    wchar_t **v = const_cast<wchar_t **>(argv);
    ::testing::InitGoogleTest(&argc, v);
    return RUN_ALL_TESTS();
}
