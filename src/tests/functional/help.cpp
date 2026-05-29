#include "test-common.h"

TEST(Help, KnownFolders_Functional) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"help", L"known-folders"
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_TRUE(p.Parse(_countof(argv), argv, run));
    EXPECT_FALSE(run);
}