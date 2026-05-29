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

    ::testing::NiceMock<MockWindowsApi> api;

    HANDLE fakeProcessHandle = reinterpret_cast<HANDLE>(0x1);
    EXPECT_CALL(api, ShellExecuteExW(
        ::testing::Pointee(
            ::testing::Field(&SHELLEXECUTEINFOW::fMask, HasBit(SEE_MASK_NOCLOSEPROCESS))
        ))).WillOnce(Invoke([fakeProcessHandle](LPSHELLEXECUTEINFOW info) {
            // ret
            info->hProcess = fakeProcessHandle;
            return TRUE;
        }));
    EXPECT_CALL(api, CloseHandle(fakeProcessHandle)).Times(1);

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
    ::testing::NiceMock<MockWindowsApi> api;

    EXPECT_CALL(api, ShellExecuteExW(_)).WillOnce(Return(FALSE));
    EXPECT_CALL(api, GetLastError()).WillOnce(Return(customError));

    // verify we got back the specific error
    EXPECT_EQ(customError, wmain_internal(
        _countof(notepad_args),
        notepad_args,
        &api
    ));
}

// Valid parameters should
// - call ShellExecuteExW
// - if ShellExecuteExW fails and GetLastError returns 0,
// return a non-zero error code
TEST(Main, ShellExecuteFailsWithZeroError) {
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    ::testing::NiceMock<MockWindowsApi> api;

    EXPECT_CALL(api, ShellExecuteExW(_)).WillOnce(Return(FALSE));
    EXPECT_CALL(api, GetLastError()).WillOnce(Return(0));

    // wmain_internal should make up a non-zero error code
    EXPECT_NE(0, wmain_internal(
        _countof(notepad_args),
        notepad_args,
        &api
    ));
}

// An invalid parameter should
// - not call ShellExecuteExW
// - return a non-zero error code
TEST(Main, InvalidParameter) {
    LPCWSTR invalid_args[] = { L"shellexecuteex.exe", L"--foo" };

    ::testing::NiceMock<MockWindowsApi> api;
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);

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
    ::testing::NiceMock<MockWindowsApi> api;
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
    ::testing::NiceMock<MockWindowsApi> api;

    const DWORD customError = 12345;
    const HANDLE customProcessHandle = reinterpret_cast<HANDLE>(0x1);

    EXPECT_CALL(api, ShellExecuteExW(_))
        .WillOnce(Invoke([customProcessHandle](SHELLEXECUTEINFOW* info){
            info->hProcess = customProcessHandle;
            return TRUE;
        }));
    EXPECT_CALL(api, WaitForSingleObject(customProcessHandle, INFINITE)).Times(1).WillOnce(Return(WAIT_OBJECT_0));
    EXPECT_CALL(api, GetExitCodeProcess(customProcessHandle, _)).Times(1).WillOnce(Return(FALSE));
    EXPECT_CALL(api, GetLastError()).Times(1).WillOnce(Return(customError));

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--no-close-process",
        L"--relay-exit-code"
    };

    EXPECT_EQ(customError, wmain_internal(
        _countof(argv),
        argv,
        &api
    ));
}

// verify relaying an exit code returns the process's exit code
TEST(Main, RelayExitCode_ChildProcess_Fails) {
    ::testing::NiceMock<MockWindowsApi> api;

    const DWORD customExitCode = 12345;
    const HANDLE customProcessHandle = reinterpret_cast<HANDLE>(0x1);
    EXPECT_CALL(api, ShellExecuteExW(_))
        .WillOnce(Invoke([customProcessHandle](SHELLEXECUTEINFOW* info){
            info->hProcess = reinterpret_cast<HANDLE>(customProcessHandle);
            return TRUE;
        }));
    EXPECT_CALL(api, WaitForSingleObject(customProcessHandle, INFINITE)).Times(1).WillOnce(Return(WAIT_OBJECT_0));
    EXPECT_CALL(api, GetExitCodeProcess(customProcessHandle, _)).Times(1)
        .WillOnce(Invoke([customExitCode](HANDLE, LPDWORD exitCode) {
            *exitCode = customExitCode;
            return TRUE;
        }));

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--no-close-process",
        L"--relay-exit-code"
    };

    EXPECT_EQ(customExitCode, wmain_internal(
        _countof(argv),
        argv,
        &api
    ));
}
