#include "test-common.h"

bool operator==(const SHELLEXECUTEINFOW &a, const SHELLEXECUTEINFOW &b) {
    return 0 == memcmp(&a, &b, sizeof(a));
}

void ExpectEq_ShellExecuteInfoW(
    const SHELLEXECUTEINFOW &expected,
    const SHELLEXECUTEINFOW &actual) {

    // before we do anything, make sure we're dealing with
    // SHELLEXECUTEINFOW-sized structs
    ASSERT_EQ(sizeof(expected), expected.cbSize);
    ASSERT_EQ(sizeof(actual), actual.cbSize);

    // it's okay if the string pointers are different
    // so long as the strings they point to are the same
    EXPECT_STREQ(expected.lpVerb, actual.lpVerb);
    EXPECT_STREQ(expected.lpFile, actual.lpFile);
    EXPECT_STREQ(expected.lpParameters, actual.lpParameters);
    EXPECT_STREQ(expected.lpDirectory, actual.lpDirectory);
    EXPECT_STREQ(expected.lpClass, actual.lpClass);

    // if other things need to be compared specially,
    // add them here
    // e.g. if two window handles point to the same window

    // make copies and clear out the string pointers
    SHELLEXECUTEINFOW e = expected;
    SHELLEXECUTEINFOW a = actual;
    e.lpVerb = a.lpVerb = nullptr;
    e.lpFile = a.lpFile = nullptr;
    e.lpParameters = a.lpParameters = nullptr;
    e.lpDirectory = a.lpDirectory = nullptr;
    e.lpClass = a.lpClass = nullptr;

    // these should now be binary identical, including padding
    EXPECT_EQ(e, a);
}

TEST(Prefs, Notepad) {
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--show", L"SW_NORMAL"
    };

    Prefs p;
    bool run = false;
    EXPECT_EQ(p.Parse(_countof(notepad_args), notepad_args, run), true);
    EXPECT_EQ(run, true);
        
    SHELLEXECUTEINFOW expected = { sizeof(expected) };
    expected.hwnd = GetConsoleWindow();
    expected.lpFile = L"notepad.exe";
    expected.nShow = SW_NORMAL;

    ExpectEq_ShellExecuteInfoW(expected, p);
}