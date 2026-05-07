#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "common.h"

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
    MOCK_METHOD(HWND, GetConsoleWindow, (), (override));
    MOCK_METHOD(BOOL, CloseHandle, (HANDLE hObject), (override));
    MOCK_METHOD(BOOL, ShellExecuteExW, (LPSHELLEXECUTEINFOW lpExecInfo), (override));
};