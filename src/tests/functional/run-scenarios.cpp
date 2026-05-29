#include "test-common.h"

TEST(Run, Cmd_Echo_1) {
    // call cmd.exe /c echo 1
    // with --no-close-process
    // and wait on it to return
    WindowsApi api;
    Prefs prefs(&api);
    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"cmd.exe",
        L"--parameters", L"/c echo 1",
        L"--show", L"SW_MINIMIZE",
        L"--no-close-process",
    };
    bool run = false;
    ASSERT_TRUE(prefs.Parse(_countof(argv), argv, run));
    ASSERT_TRUE(run);

    BOOL result = api.ShellExecuteExW(&prefs);
    ASSERT_TRUE(result);

    ASSERT_NE(nullptr, prefs.hProcess);
    EXPECT_EQ(WAIT_OBJECT_0, WaitForSingleObject(prefs.hProcess, INFINITE));
}

TEST(Run, Cmd_Exit_12345) {
    // call cmd.exe /c exit 12345
    // with --no-close-process
    // wait on it to return
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
    EXPECT_EQ(12345, wmain_internal(
        _countof(argv),
        argv,
        &api
    ));
}

TEST(Run, Invalid) {
    // call foo
    WindowsApi api;
    Prefs prefs(&api);
    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"foo",
        L"--no-ui"
    };
    bool run = false;
    EXPECT_NE(0, wmain_internal(
        _countof(argv),
        argv,
        &api
    ));
}
