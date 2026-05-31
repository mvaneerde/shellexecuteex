#include "test-common.h"

TEST(Main, CoInitializeCalled) {
    // verify COM is initialized
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    ::testing::NiceMock<MockWindowsApi> api;

    EXPECT_CALL(api, CoInitializeEx(_, _)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(1).WillOnce(Return(TRUE));
    EXPECT_CALL(api, CoUninitialize()).Times(1);

    EXPECT_EQ(0, wmain_internal(
        _countof(notepad_args),
        notepad_args,
        &api
    ));
}

TEST(Main, CoUnInitializeSkippedOnFailure) {
    // if CoInitialize fails, don't call CoUninitialize
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    HRESULT failure = E_FAIL;

    ::testing::NiceMock<MockWindowsApi> api;
    EXPECT_CALL(api, CoInitializeEx(_, _)).Times(1).WillOnce(Invoke([failure](LPVOID, DWORD) { return failure; }));
    EXPECT_CALL(api, CoUninitialize()).Times(0);

    EXPECT_EQ(failure, wmain_internal(
        _countof(notepad_args),
        notepad_args,
        &api
    ));
}
