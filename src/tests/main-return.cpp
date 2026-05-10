#include "test-common.h"

// Valid parameters should
// - call ShellExecuteExW
// - if ShellExecuteExW succeeds, return 0
TEST(Main, ShellExecuteSucceeds) {
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--no-close-process"
    };

    MockWindowsApi api;

    EXPECT_CALL(api, GetConsoleWindow()).WillOnce(Invoke(::GetConsoleWindow));
    EXPECT_CALL(api, ShellExecuteExW(_))
        .WillOnce(Invoke([](LPSHELLEXECUTEINFOW info) {
            // create a closable handle
            // without actually launching anything
            if ((info->fMask & SEE_MASK_NOCLOSEPROCESS) == SEE_MASK_NOCLOSEPROCESS) {
                info->hProcess = CreateEvent(nullptr, TRUE, FALSE, nullptr);
            }
            return TRUE;
        }));
    EXPECT_CALL(api, CloseHandle(_)).WillOnce(Invoke(::CloseHandle));

    EXPECT_EQ(0, wmain_internal(
        _countof(notepad_args),
        notepad_args,
        &api
    ));
}

// Valid parameters should
// - call ShellExecuteExW
// - if ShellExecuteExW fails, return the error code it sets
TEST(Main, ShellExecuteFails) {
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    const DWORD customError = 123456789;
    MockWindowsApi api;

    EXPECT_CALL(api, GetConsoleWindow()).WillOnce(Invoke(::GetConsoleWindow));
    EXPECT_CALL(api, ShellExecuteExW(_))
        .WillOnce(Invoke([customError](LPSHELLEXECUTEINFOW) {
            // simulate failure with a custom error code
            SetLastError(customError);
            return FALSE;
        }));
    EXPECT_CALL(api, CloseHandle(_)).Times(0);

    // verify we got back the specific error
    EXPECT_EQ(customError, wmain_internal(
        _countof(notepad_args),
        notepad_args,
        &api
    ));
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

    MockWindowsApi api;

    for (auto a : args) {
        EXPECT_CALL(api, GetConsoleWindow()).WillOnce(Invoke(::GetConsoleWindow));
        EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);
        EXPECT_CALL(api, CloseHandle(_)).Times(0);
        
        EXPECT_EQ(0, wmain_internal(
            a.argc,
            a.argv,
            &api
        ));
    }
}

// An invalid parameter should
// - not call ShellExecuteExW
// - return a non-zero error code
TEST(Main, InvalidParameter) {
    LPCWSTR invalid_args[] = { L"shellexecuteex.exe", L"--foo" };

    MockWindowsApi api;
    EXPECT_CALL(api, GetConsoleWindow()).WillOnce(Invoke(::GetConsoleWindow));
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);
    EXPECT_CALL(api, CloseHandle(_)).Times(0);

    // any non-zero error code is fine
    EXPECT_NE(0, wmain_internal(
        _countof(invalid_args),
        invalid_args,
        &api
    ));
}

// if we ask to relay the exit code
// but the process handle is null
// then main should return a failure
TEST(Main, RelayExitCode_NullProcessHandle) {
    MockWindowsApi api;
    EXPECT_CALL(api, CoInitializeEx(_, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(api, CloseHandle(_)).Times(0);
    EXPECT_CALL(api, CoUninitialize()).Times(1);
    EXPECT_CALL(api, GetConsoleWindow()).Times(1);
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);
    EXPECT_CALL(api, ShellExecuteExW(_)).WillOnce(Return(TRUE));

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--no-close-process",
        L"--relay-exit-code"
    };

    EXPECT_NE(0, wmain_internal(
        _countof(argv),
        argv,
        &api
    ));
}

// verify relaying an exit code returns a failure
// if GetExitCodeProcess fails
TEST(Main, RelayExitCode_GetExitCodeProcess_Fails) {
    MockWindowsApi api;

    EXPECT_CALL(api, CoInitializeEx(_, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(api, CloseHandle(_)).Times(1);
    EXPECT_CALL(api, CoUninitialize()).Times(1);
    EXPECT_CALL(api, GetConsoleWindow()).Times(1);
    EXPECT_CALL(api, GetExitCodeProcess(_, _))
        .WillOnce(Invoke([](HANDLE, LPDWORD exitCode) {
            SetLastError(12345);
            return FALSE;
        }));
    EXPECT_CALL(api, ShellExecuteExW(_))
        .WillOnce(Invoke([](SHELLEXECUTEINFOW* info){
            info->hProcess = reinterpret_cast<HANDLE>(0x1);
            return TRUE;
        }));
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(1).WillOnce(Return(WAIT_OBJECT_0));

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--no-close-process",
        L"--relay-exit-code"
    };

    EXPECT_EQ(12345, wmain_internal(
        _countof(argv),
        argv,
        &api
    ));
}

// verify relaying an exit code returns the process's exit code
TEST(Main, RelayExitCode_ChildProcess_Fails) {
    MockWindowsApi api;

    EXPECT_CALL(api, CoInitializeEx(_, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(api, CloseHandle(_)).Times(1);
    EXPECT_CALL(api, CoUninitialize()).Times(1);
    EXPECT_CALL(api, GetConsoleWindow()).Times(1);
    EXPECT_CALL(api, GetExitCodeProcess(_, _))
        .WillOnce(Invoke([](HANDLE, LPDWORD exitCode) {
            *exitCode = 12345;
            return TRUE;
        }));
    EXPECT_CALL(api, ShellExecuteExW(_))
        .WillOnce(Invoke([](SHELLEXECUTEINFOW* info){
            info->hProcess = reinterpret_cast<HANDLE>(0x1);
            return TRUE;
        }));
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(1).WillOnce(Return(WAIT_OBJECT_0));

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--no-close-process",
        L"--relay-exit-code"
    };

    EXPECT_EQ(12345, wmain_internal(
        _countof(argv),
        argv,
        &api
    ));
}
