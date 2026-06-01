#include "test-common.h"

TEST(Run, Cmd_Relay) {
    // call cmd.exe /c exit 12345
    // with --relay-exit-code
    // verify the exit code from main is 12345
    WindowsApi api;
    Prefs prefs(&api);
    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"cmd.exe",
        L"--parameters", L"/c exit 12345",
        L"--show", L"SW_MINIMIZE",
        L"--no-close-process",
        L"--relay-exit-code"
    };
    EXPECT_EQ(12345, wmain_testable(
        _countof(argv),
        argv
    ));
}

TEST(Run, Invalid) {
    // call foo
    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"foo",
        L"--no-ui"
    };
    bool run = false;
    EXPECT_EQ(ERROR_FILE_NOT_FOUND, wmain_testable(
        _countof(argv),
        argv
    ));
}
