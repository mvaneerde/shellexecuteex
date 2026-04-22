#include "test-common.h"

// set this to false
// then attempt to invoke MockShellExecuteExW
//
// if it's true afterwards, MockShellExecuteExW was invoked
bool g_shellExecuteCalled = false;

// make a MockShellExecuteExW that behaves however you want
template<BOOL forceResult, DWORD forceError>
BOOL MockShellExecuteExW(LPSHELLEXECUTEINFOW info) {
    LOG(L"Inside MockShellExecuteExW<forceResult=%d, forceError=%d>", forceResult, forceError);

    // report that we were invoked
    g_shellExecuteCalled = true;

    // behave as directed
    if (!forceResult) {
        SetLastError(forceError);
    }

    return forceResult;
}

// Valid parameters should
// - call ShellExecuteExW
// - if ShellExecuteExW succeeds, return 0
TEST(Main, ShellExecuteSucceeds) {
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show", L"SW_NORMAL"
    };

    g_shellExecuteCalled = false;
    EXPECT_EQ(0, wmain_internal(
        _countof(notepad_args),
        notepad_args,
        MockShellExecuteExW<TRUE, ERROR_SUCCESS>
    ));
    EXPECT_TRUE(g_shellExecuteCalled);
    // no expectation on GetLastError()
}

// Valid parameters should
// - call ShellExecuteExW
// - if ShellExecuteExW fails, return the error code it sets
TEST(Main, ShellExecuteFails) {
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show", L"SW_NORMAL"
    };

    const DWORD customError = 123456789;

    g_shellExecuteCalled = false;
    EXPECT_EQ(customError, wmain_internal(
        _countof(notepad_args),
        notepad_args,
        MockShellExecuteExW<FALSE, customError>
    ));
    EXPECT_TRUE(g_shellExecuteCalled);
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
            MockShellExecuteExW<TRUE, ERROR_SUCCESS>
        ));
        EXPECT_FALSE(g_shellExecuteCalled);
    }
}

// An invalid parameter should
// - not call ShellExecuteExW
// - return a non-zero error code
TEST(Main, InvalidParameter) {
    LPCWSTR invalid_args[] = { L"shellexecuteex.exe", L"--foo" };

    g_shellExecuteCalled = false;
    // any non-zero error code is fine
    EXPECT_NE(0, wmain_internal(
        _countof(invalid_args),
        invalid_args,
        MockShellExecuteExW<TRUE, ERROR_SUCCESS>
    ));
    EXPECT_FALSE(g_shellExecuteCalled);
}
