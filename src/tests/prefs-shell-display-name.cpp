#include "test-common.h"

TEST(Prefs, ShellDisplayName_Missing_Argument) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--shell-display-name",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_FALSE(p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ShellDisplayName_And_File) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--shell-display-name", L"::{645FF040-5081-101B-9F08-00AA002F954E}",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_FALSE(p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ShellDisplayName_Twice) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--shell-display-name", L"::{645FF040-5081-101B-9F08-00AA002F954E}",
        L"--shell-display-name", L"::{645FF040-5081-101B-9F08-00AA002F954E}",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_FALSE(p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ShellDisplayName_CanonicalName) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        // canonical name for the Recycle Bin folder
        L"--shell-display-name", L"::{645FF040-5081-101B-9F08-00AA002F954E}",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_TRUE(p.Parse(_countof(argv), argv, run));
    EXPECT_TRUE(run);
    EXPECT_EQ(SEE_MASK_IDLIST, p.fMask);
    EXPECT_NE(nullptr, p.lpIDList);
}


TEST(Prefs, ShellDisplayName_DisplayName) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--shell-display-name", L"shell:RecycleBinFolder",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_TRUE(p.Parse(_countof(argv), argv, run));
    EXPECT_TRUE(run);
    EXPECT_EQ(SEE_MASK_IDLIST, p.fMask);
    EXPECT_NE(nullptr, p.lpIDList);
}
