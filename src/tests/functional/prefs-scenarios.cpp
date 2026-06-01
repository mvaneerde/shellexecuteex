#include "test-common.h"

TEST(Prefs, Calc) {
    LPCWSTR args[] = {
        L"shellexecuteex.exe",
        L"--file", L"calc.exe"
    };

    WindowsApi api;
    Prefs p(&api);
    EXPECT_EQ(S_OK, p.Parse(_countof(args), args));
        
    SHELLEXECUTEINFOW expected = { sizeof(expected) };
    expected.hwnd = GetConsoleWindow();
    expected.lpFile = L"calc.exe";
    expected.nShow = SW_NORMAL;

    LPSHELLEXECUTEINFO actual = p.GetShellExecuteInfo();

    ExpectEq_ShellExecuteInfoW(expected, *actual);
}

TEST(Prefs, Command_ListCsvFiles) {
    LPCWSTR args[] = {
        L"shellexecuteex.exe",
        L"--file", L"cmd.exe",
        L"--parameters", L"/c dir /b *.csv > csv-files.txt",
        L"--directory", L"%userprofile%\\Downloads",
        L"--do-environment-substitution",
        L"--show", L"SW_SHOWMINIMIZED"
    };

    WindowsApi api;
    Prefs p(&api);
    EXPECT_EQ(S_OK, p.Parse(_countof(args), args));
        
    SHELLEXECUTEINFOW expected = { sizeof(expected) };
    expected.hwnd = GetConsoleWindow();
    expected.fMask = SEE_MASK_DOENVSUBST;
    expected.lpFile = L"cmd.exe";
    expected.lpParameters = L"/c dir /b *.csv > csv-files.txt";
    expected.lpDirectory = L"%userprofile%\\Downloads";
    expected.nShow = SW_SHOWMINIMIZED;

    LPSHELLEXECUTEINFO actual = p.GetShellExecuteInfo();

    ExpectEq_ShellExecuteInfoW(expected, *actual);
}

TEST(Prefs, Admin_PowerShell) {
    LPCWSTR args[] = {
        L"shellexecuteex.exe",
        L"--verb", L"runas",
        L"--file", L"powershell.exe"
    };

    WindowsApi api;
    Prefs p(&api);
    EXPECT_EQ(S_OK, p.Parse(_countof(args), args));
        
    SHELLEXECUTEINFOW expected = { sizeof(expected) };
    expected.hwnd = GetConsoleWindow();
    expected.lpVerb = L"runas";
    expected.lpFile = L"powershell.exe";
    expected.nShow = SW_NORMAL;

    LPSHELLEXECUTEINFO actual = p.GetShellExecuteInfo();

    ExpectEq_ShellExecuteInfoW(expected, *actual);
}
