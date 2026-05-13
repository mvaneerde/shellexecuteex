#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "common.h"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

struct Args {
    int argc;
    LPCWSTR *argv;

    Args(int c, LPCWSTR v[]) : argc(c), argv(v) {}
};

bool operator==(const SHELLEXECUTEINFOW &a, const SHELLEXECUTEINFOW &b);
void ExpectEq_ShellExecuteInfoW(
    const SHELLEXECUTEINFOW &expected,
    const SHELLEXECUTEINFOW &actual);
    
class MockWindowsApi : public IWindowsApi {
public:
    MOCK_METHOD(HRESULT, CoInitializeEx, (LPVOID reserved, DWORD coinit), (override));
    MOCK_METHOD(BOOL, CloseHandle, (HANDLE h), (override));
    MOCK_METHOD(void, CoTaskMemFree, (PVOID p), (override));
    MOCK_METHOD(void, CoUninitialize, (), (override));
    MOCK_METHOD(HWND, GetConsoleWindow, (), (override));
    MOCK_METHOD(BOOL, GetExitCodeProcess, (HANDLE process, LPDWORD exitCode), (override));
    MOCK_METHOD(BOOL, ShellExecuteExW, (LPSHELLEXECUTEINFOW info), (override));
    MOCK_METHOD(HRESULT, SHParseDisplayName, (
        PCWSTR name,
        IBindCtx *context,
        PIDLIST_ABSOLUTE *idlist,
        SFGAOF in,
        SFGAOF *out
    ), (override));
    MOCK_METHOD(DWORD, WaitForSingleObject, (HANDLE h, DWORD milliseconds), (override));
};
