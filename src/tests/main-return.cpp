#include "test-common.h"



// set these to false
// then check to see if they were flipped to true
bool g_closeHandleCalled = false;
bool g_getConsoleWindowCalled = false;
bool g_shellExecuteCalled = false;

template <BOOL forceResult, DWORD forceError> class MockShellExecuteEx : public IWindowsApi {
public:
    BOOL CloseHandle(HANDLE h) override {
        // report that we were invoked
        g_closeHandleCalled = true;
        return ::CloseHandle(h);
    }

    HWND GetConsoleWindow() override {
        g_getConsoleWindowCalled = true;
        return ::GetConsoleWindow();
    }

    BOOL ShellExecuteEx(LPSHELLEXECUTEINFO info) override {
        // report that we were invoked
        g_shellExecuteCalled = true;

        // behave as directed
        if (forceResult) {
            if ((info->fMask & SEE_MASK_NOCLOSEPROCESS) == SEE_MASK_NOCLOSEPROCESS) {
                info->hProcess = CreateEvent(nullptr, TRUE, FALSE, nullptr);
            }
        } else {
            SetLastError(forceError);
        }

        return forceResult;
    }
};

// Valid parameters should
// - call ShellExecuteExW
// - if ShellExecuteExW succeeds, return 0
TEST(Main, ShellExecuteSucceeds) {
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show", L"SW_NORMAL",
        L"--no-close-process"
    };

    MockShellExecuteEx<TRUE, ERROR_SUCCESS> api;

    g_closeHandleCalled = false;
    g_getConsoleWindowCalled = false;
    g_shellExecuteCalled = false;
    EXPECT_EQ(0, wmain_internal(
        _countof(notepad_args),
        notepad_args,
        &api
    ));
    EXPECT_TRUE(g_closeHandleCalled);
    EXPECT_TRUE(g_getConsoleWindowCalled);
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
    MockShellExecuteEx<FALSE, customError> api;

    g_closeHandleCalled = false;
    g_getConsoleWindowCalled = false;
    g_shellExecuteCalled = false;
    EXPECT_EQ(customError, wmain_internal(
        _countof(notepad_args),
        notepad_args,
        &api
    ));
    EXPECT_FALSE(g_closeHandleCalled);
    EXPECT_TRUE(g_getConsoleWindowCalled);
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

    MockShellExecuteEx<TRUE, ERROR_SUCCESS> api;

    for (auto a : args) {
        g_closeHandleCalled = false;
        g_getConsoleWindowCalled = false;
        g_shellExecuteCalled = false;
        EXPECT_EQ(0, wmain_internal(
            a.argc,
            a.argv,
            &api
        ));
        EXPECT_FALSE(g_closeHandleCalled);
        EXPECT_TRUE(g_getConsoleWindowCalled);
        EXPECT_FALSE(g_shellExecuteCalled);
    }
}

// An invalid parameter should
// - not call ShellExecuteExW
// - return a non-zero error code
TEST(Main, InvalidParameter) {
    LPCWSTR invalid_args[] = { L"shellexecuteex.exe", L"--foo" };

    MockShellExecuteEx<TRUE, ERROR_SUCCESS> api;

    g_closeHandleCalled = false;
    g_getConsoleWindowCalled = false;
    g_shellExecuteCalled = false;

    // any non-zero error code is fine
    EXPECT_NE(0, wmain_internal(
        _countof(invalid_args),
        invalid_args,
        &api
    ));
    EXPECT_FALSE(g_closeHandleCalled);
    EXPECT_TRUE(g_getConsoleWindowCalled);
    EXPECT_FALSE(g_shellExecuteCalled);
}
