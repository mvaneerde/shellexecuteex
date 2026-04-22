#include "test-common.h"

TEST(Prefs, ShowValid) {
    // result and run should both be true
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show", L"SW_NORMAL"
    };

    Prefs p;
    bool run = false; // Parse should set this to "true"
    EXPECT_EQ(p.Parse(_countof(notepad_args), notepad_args, run), true);
    EXPECT_EQ(run, true);
}

TEST(Prefs, ShowDuplicate) {
    // it should be an error to specify --show multiple times
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show", L"SW_NORMAL",
        L"--show", L"SW_NORMAL"
    };

    Prefs p;
    bool run = false; // Parse should set this to "true"
    EXPECT_EQ(p.Parse(_countof(notepad_args), notepad_args, run), false);
    // no expectation on "run"
}

TEST(Prefs, ShowInvalid) {
    // it should be an error to specify an invalid value
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show", L"foo",
    };

    Prefs p;
    bool run = false;
    EXPECT_EQ(p.Parse(_countof(notepad_args), notepad_args, run), false);
    // no expectation on "run"
}

TEST(Prefs, ShowMissingValue) {
    // --show requires a value
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show"
    };

    Prefs p;
    bool run = false;
    bool result = p.Parse(_countof(notepad_args), notepad_args, run);

    EXPECT_EQ(result, false);
    // no expectation on "run"
}

TEST(Prefs, ShowMissing) {
    // --show is required
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    Prefs p;
    bool run = false;
    bool result = p.Parse(_countof(notepad_args), notepad_args, run);

    EXPECT_EQ(result, false);
    // no expectation on "run"
}
