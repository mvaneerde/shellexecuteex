#include "test-common.h"

bool g_shellExecuteCalled = false;

BOOL MockShellExecuteExW(LPSHELLEXECUTEINFOW info) {
    LOG(L"%s", L"Inside MockShellExecuteExW");

    g_shellExecuteCalled = true;

    return TRUE;
}

// Invoking a usage statement should
// - not call ShellExecuteExW
// - return 0
TEST(Main, Usage) {
    std::vector<Args> args;

    // all of these sets of arguments should trigger a usage statement
    // with a success return
    // and a "do not run anything" output
    LPCWSTR no_args[] = { L"shellexecuteex.exe" };
    args.push_back(Args(_countof(no_args), no_args));

    LPCWSTR help[][2] = {
        { L"shellexecuteex.exe", L"-?" },
        { L"shellexecuteex.exe", L"/?" },
        { L"shellexecuteex.exe", L"--help" },
    };
    for (int i = 0; i < _countof(help); i++) {
        args.push_back(Args(_countof(help[i]), help[i]));
    }

    for (auto a : args) {
        g_shellExecuteCalled = false;
        EXPECT_EQ(0, wmain_internal(
            a.argc,
            a.argv,
            MockShellExecuteExW
        ));
        EXPECT_FALSE(g_shellExecuteCalled);
    }
}
