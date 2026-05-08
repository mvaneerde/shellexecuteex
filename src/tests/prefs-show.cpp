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
    bool run = false; // Parse should set this to "true"
    EXPECT_TRUE(p.Parse(_countof(notepad_args), notepad_args, run));
    EXPECT_TRUE(run);
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
    bool run = false; // Parse should set this to "true"
    EXPECT_EQ(false, p.Parse(_countof(notepad_args), notepad_args, run));
    // no expectation on "run"
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
    bool run = false;
    EXPECT_EQ(false, p.Parse(_countof(notepad_args), notepad_args, run));
    // no expectation on "run"
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
    bool run = false;
    bool result = p.Parse(_countof(notepad_args), notepad_args, run);

    EXPECT_FALSE(result);
    // no expectation on "run"
}

TEST(Prefs, ShowDefault) {
    // --show defaults to SW_NORMAL
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    WindowsApi api;
    Prefs p(&api);
    bool run = false;
    bool result = p.Parse(_countof(notepad_args), notepad_args, run);

    EXPECT_TRUE(result);
    EXPECT_TRUE(run);
    EXPECT_EQ(SW_NORMAL, p.nShow);
}
