#include "test-common.h"

TEST(ShowWindowCommand, Parse_Invalid) {
    int command = -1;
    EXPECT_EQ(E_INVALIDARG, ShowWindowCommand::ParseShowWindowCommand(L"foo", command));
    // no expectation on command
}

TEST(ShowWindowCommand, Parse_Valid) {
    int command = -1;
    EXPECT_EQ(S_OK, ShowWindowCommand::ParseShowWindowCommand(L"SW_NORMAL", command));
    EXPECT_EQ(SW_NORMAL, command);
}
