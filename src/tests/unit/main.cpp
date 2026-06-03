#include "test-common.h"

TEST(Main, Api_CoInitializeEx_Failed) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    int failure = -12345;
    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(failure));
    
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(0);
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(0);

    EXPECT_CALL(prefs, Parse(_, _)).Times(0);
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(0);
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);
    EXPECT_CALL(api, GetLastError()).Times(0);
    EXPECT_CALL(prefs, RelayExitCode()).Times(0);
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(0);
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(0);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(failure, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Usage_HandleUsage_Failed) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));

    // these might be called in either order but we want HandleUsage to fail
    HRESULT failure = -12345;
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [failure](int, LPCWSTR[], bool &handled) -> HRESULT {
            return failure;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(::testing::Between(0, 1)).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));

    EXPECT_CALL(prefs, Parse(_, _)).Times(0);
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(0);
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);
    EXPECT_CALL(api, GetLastError()).Times(0);
    EXPECT_CALL(prefs, RelayExitCode()).Times(0);
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(0);
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(failure, wmain_mockable(_countof(argv), argv, context));
}


TEST(Main, Usage_Handle_Usage_Handled) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));

    // these might be called in either order but we want HandleUsage to handle it
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = true;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(::testing::Between(0, 1)).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));

    EXPECT_CALL(prefs, Parse(_, _)).Times(0);
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(0);
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);
    EXPECT_CALL(api, GetLastError()).Times(0);
    EXPECT_CALL(prefs, RelayExitCode()).Times(0);
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(0);
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(0, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Usage_HandleHelp_Failed) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));

    // these might be called in either order but we want HandleHelp to fail
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(::testing::Between(0, 1)).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    HRESULT failure = -12345;
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [failure](int, LPCWSTR[], bool &handled) -> HRESULT {
            return failure;
        }
    ));

    EXPECT_CALL(prefs, Parse(_, _)).Times(0);
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(0);
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);
    EXPECT_CALL(api, GetLastError()).Times(0);
    EXPECT_CALL(prefs, RelayExitCode()).Times(0);
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(0);
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(failure, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Usage_HandleHelp_Handled) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));

    // these might be called in either order but we want HandleHelp to handle it
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(::testing::Between(0, 1)).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = true;
            return S_OK;
        }
    ));

    EXPECT_CALL(prefs, Parse(_, _)).Times(0);
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(0);
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);
    EXPECT_CALL(api, GetLastError()).Times(0);
    EXPECT_CALL(prefs, RelayExitCode()).Times(0);
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(0);
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(0, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Prefs_Parse_Failed) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));

    HRESULT failure = -12345;
    EXPECT_CALL(prefs, Parse(_, _)).Times(1).WillOnce(Return(failure));

    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(0);
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);
    EXPECT_CALL(api, GetLastError()).Times(0);
    EXPECT_CALL(prefs, RelayExitCode()).Times(0);
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(0);
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(failure, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, No_Relay_Exit_Code) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(prefs, Parse(_, _)).Times(1).WillOnce(Return(S_OK));

    SHELLEXECUTEINFOW info = {};
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(::testing::AtLeast(1)).WillRepeatedly(Return(&info));

    HANDLE fakeHandle = reinterpret_cast<HANDLE>(0x1234);
    EXPECT_CALL(api, ShellExecuteExW(&info)).Times(1).WillOnce(Invoke(
        [fakeHandle](LPSHELLEXECUTEINFOW info) -> BOOL {
            info->hProcess = fakeHandle;
            return TRUE;
        }
    ));
    EXPECT_CALL(api, GetLastError()).Times(0);
    EXPECT_CALL(prefs, RelayExitCode()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(0);
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(0, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Api_ShellExecuteEx_Failed) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(prefs, Parse(_, _)).Times(1).WillOnce(Return(S_OK));

    SHELLEXECUTEINFOW info = {};
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(::testing::AtLeast(1)).WillRepeatedly(Return(&info));

    EXPECT_CALL(api, ShellExecuteExW(&info)).Times(1).WillOnce(Return(FALSE));

    DWORD failure = 12345;
    EXPECT_CALL(api, GetLastError()).Times(1).WillOnce(Return(failure));
    EXPECT_CALL(prefs, RelayExitCode()).Times(0);
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(0);
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(failure, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Relay_Exit_Code_Null_Process) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(prefs, Parse(_, _)).Times(1).WillOnce(Return(S_OK));

    SHELLEXECUTEINFOW info = {};
    info.hProcess = reinterpret_cast<HANDLE>(0x1234);
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(::testing::AtLeast(1)).WillRepeatedly(Return(&info));

    EXPECT_CALL(api, ShellExecuteExW(&info)).Times(1).WillOnce(Invoke(
        [](LPSHELLEXECUTEINFOW info) -> BOOL {
            info->hProcess = nullptr;
            return TRUE;
        }
    ));

    EXPECT_CALL(api, GetLastError()).Times(0);
    EXPECT_CALL(prefs, RelayExitCode()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(0);
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(ERROR_INVALID_HANDLE, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Relay_Exit_Code_Wait_Failed) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(prefs, Parse(_, _)).Times(1).WillOnce(Return(S_OK));

    SHELLEXECUTEINFOW info = {};
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(::testing::AtLeast(1)).WillRepeatedly(Return(&info));

    HANDLE fakeHandle = reinterpret_cast<HANDLE>(0x1234);
    EXPECT_CALL(api, ShellExecuteExW(&info)).Times(1).WillOnce(Invoke(
        [fakeHandle](LPSHELLEXECUTEINFOW info) -> BOOL {
            info->hProcess = fakeHandle;
            return TRUE;
        }
    ));

    EXPECT_CALL(prefs, RelayExitCode()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(1).WillOnce(Return(WAIT_FAILED));

    DWORD failure = 12345;
    EXPECT_CALL(api, GetLastError()).Times(1).WillOnce(Return(failure));
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(failure, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Relay_Exit_Code_Wait_Failed_No_Error) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(prefs, Parse(_, _)).Times(1).WillOnce(Return(S_OK));

    SHELLEXECUTEINFOW info = {};
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(::testing::AtLeast(1)).WillRepeatedly(Return(&info));

    HANDLE fakeHandle = reinterpret_cast<HANDLE>(0x1234);
    EXPECT_CALL(api, ShellExecuteExW(&info)).Times(1).WillOnce(Invoke(
        [fakeHandle](LPSHELLEXECUTEINFOW info) -> BOOL {
            info->hProcess = fakeHandle;
            return TRUE;
        }
    ));

    EXPECT_CALL(prefs, RelayExitCode()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(1).WillOnce(Return(WAIT_FAILED));

    EXPECT_CALL(api, GetLastError()).Times(1).WillOnce(Return(ERROR_SUCCESS));
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(0);

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    // main should return a non-zero code (any code will do)
    EXPECT_NE(0, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Relay_Exit_Code_GetExitCodeProcess_Failed) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(prefs, Parse(_, _)).Times(1).WillOnce(Return(S_OK));

    SHELLEXECUTEINFOW info = {};
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(::testing::AtLeast(1)).WillRepeatedly(Return(&info));

    HANDLE fakeHandle = reinterpret_cast<HANDLE>(0x1234);
    EXPECT_CALL(api, ShellExecuteExW(&info)).Times(1).WillOnce(Invoke(
        [fakeHandle](LPSHELLEXECUTEINFOW info) -> BOOL {
            info->hProcess = fakeHandle;
            return TRUE;
        }
    ));

    EXPECT_CALL(prefs, RelayExitCode()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(1).WillOnce(Return(WAIT_OBJECT_0));
    
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(1).WillOnce(Return(FALSE));

    DWORD failure = 12345;
    EXPECT_CALL(api, GetLastError()).Times(1).WillOnce(Return(failure));

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(failure, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Relay_Exit_Code_GetExitCodeProcess_Failed_No_Error) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(prefs, Parse(_, _)).Times(1).WillOnce(Return(S_OK));

    SHELLEXECUTEINFOW info = {};
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(::testing::AtLeast(1)).WillRepeatedly(Return(&info));

    HANDLE fakeHandle = reinterpret_cast<HANDLE>(0x1234);
    EXPECT_CALL(api, ShellExecuteExW(&info)).Times(1).WillOnce(Invoke(
        [fakeHandle](LPSHELLEXECUTEINFOW info) -> BOOL {
            info->hProcess = fakeHandle;
            return TRUE;
        }
    ));

    EXPECT_CALL(prefs, RelayExitCode()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(1).WillOnce(Return(WAIT_OBJECT_0));
    
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(1).WillOnce(Return(FALSE));

    EXPECT_CALL(api, GetLastError()).Times(1).WillOnce(Return(ERROR_SUCCESS));

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    // main should return a non-zero code (any code will do)
    EXPECT_NE(0, wmain_mockable(_countof(argv), argv, context));
}

TEST(Main, Relay_Exit_Code_Valid) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockUsage> usage;
    ::testing::NiceMock<MockPrefs> prefs;
    MainContext context = { &api, &knownFolders, &usage, &prefs };

    EXPECT_CALL(api, CoInitializeEx(nullptr, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(usage, HandleUsage(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(usage, HandleHelp(_, _, _)).Times(1).WillOnce(Invoke(
        [](int, LPCWSTR[], bool &handled) -> HRESULT {
            handled = false;
            return S_OK;
        }
    ));
    EXPECT_CALL(prefs, Parse(_, _)).Times(1).WillOnce(Return(S_OK));

    SHELLEXECUTEINFOW info = {};
    EXPECT_CALL(prefs, GetShellExecuteInfo()).Times(::testing::AtLeast(1)).WillRepeatedly(Return(&info));

    HANDLE fakeHandle = reinterpret_cast<HANDLE>(0x1234);
    EXPECT_CALL(api, ShellExecuteExW(&info)).Times(1).WillOnce(Invoke(
        [fakeHandle](LPSHELLEXECUTEINFOW info) -> BOOL {
            info->hProcess = fakeHandle;
            return TRUE;
        }
    ));
    EXPECT_CALL(api, GetLastError()).Times(0);
    EXPECT_CALL(prefs, RelayExitCode()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(api, WaitForSingleObject(_, _)).Times(1).WillOnce(Return(WAIT_OBJECT_0));
    
    DWORD failure = 12345;
    EXPECT_CALL(api, GetExitCodeProcess(_, _)).Times(1).WillOnce(Invoke(
        [failure](HANDLE, LPDWORD exitCode) -> BOOL {
            *exitCode = failure;
            return TRUE;
        }
    ));

    EXPECT_CALL(api, CoUninitialize()).Times(1);

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    EXPECT_EQ(failure, wmain_mockable(_countof(argv), argv, context));
}
