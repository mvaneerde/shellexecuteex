#include "test-common.h"

TEST(Prefs, InvalidArgument) {
    ::testing::NiceMock<MockWindowsApi> api;
    Prefs prefs(&api);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe", L"no-such-arg"
    };

    EXPECT_EQ(E_INVALIDARG, prefs.Parse(_countof(argv), argv));
}
