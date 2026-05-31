#include "test-common.h"

TEST(Prefs, ShowValid) {
    // result and run should both be true
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show", L"SW_SHOWMAXIMIZED"
     };

    WindowsApi api;
    Prefs p(&api);
    EXPECT_EQ(S_OK, p.Parse(_countof(notepad_args), notepad_args));
}

TEST(Prefs, ShowDuplicate) {
    // it should be an error to specify --show multiple times
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show", L"SW_NORMAL",
        L"--show", L"SW_NORMAL"
    };

    WindowsApi api;
    Prefs p(&api);
    EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(notepad_args), notepad_args));
}

TEST(Prefs, ShowInvalid) {
    // it should be an error to specify an invalid value
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show", L"foo",
    };

    WindowsApi api;
    Prefs p(&api);
    EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(notepad_args), notepad_args));
}

TEST(Prefs, ShowMissingValue) {
    // --show requires a value
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show"
    };

    WindowsApi api;
    Prefs p(&api);
    EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(notepad_args), notepad_args));
}

TEST(Prefs, ShowDefault) {
    // --show defaults to SW_NORMAL
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    WindowsApi api;
    Prefs p(&api);
    EXPECT_EQ(S_OK, p.Parse(_countof(notepad_args), notepad_args));
    EXPECT_EQ(SW_NORMAL, p.nShow);
}
