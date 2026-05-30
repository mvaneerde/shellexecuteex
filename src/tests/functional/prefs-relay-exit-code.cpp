#include "test-common.h"

TEST(Prefs, RelayExitCode_Missing) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_EQ(S_OK,p.Parse(_countof(argv), argv, run));
    EXPECT_TRUE(run);

    EXPECT_FALSE(p.RelayExitCode());
}

TEST(Prefs, RelayExitCode_With_NoCloseProcess) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--no-close-process",
        L"--relay-exit-code"
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_EQ(S_OK, p.Parse(_countof(argv), argv, run));
    EXPECT_TRUE(run);

    EXPECT_TRUE(p.RelayExitCode());
}

TEST(Prefs, RelayExitCode_Duplicate) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--relay-exit-code",
        L"--relay-exit-code"
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, RelayExitCode_Without_NoCloseProcess) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--relay-exit-code"
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(argv), argv, run));
    // no expectation on run
}
