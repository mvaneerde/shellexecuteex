#include "test-common.h"

// String arguments
TEST(Prefs, StringArguments) {
    SHELLEXECUTEINFOW expected = { sizeof(expected) };
    expected.lpFile = L"notepad.exe";
    expected.nShow = SW_NORMAL;

    std::tuple<LPCWSTR, LPCWSTR*, DWORD> stringArgs[] = {
        {
            L"--verb",
            &expected.lpVerb,
            0
        },
        {
            L"--parameters",
            &expected.lpParameters,
            0
        },
        {
            L"--directory",
            &expected.lpDirectory,
            0
        },
        {
            L"--class-name",
            &expected.lpClass,
            SEE_MASK_CLASSNAME
        },
    };

    for (auto a : stringArgs) {
        // valid case - passing the argument
        // sets the string and the flag
        {
            LPCWSTR argv[] = {
                L"ShellExecuteEx.exe",
                L"--file", L"notepad.exe",
                L"--show", L"SW_NORMAL",
                std::get<0>(a), L"foo"
            };

            Prefs p;
            bool run = false;
            EXPECT_EQ(true, p.Parse(_countof(argv), argv, run));
            EXPECT_EQ(true, run);

            LPCWSTR oldValue = *std::get<1>(a);

            *std::get<1>(a) = L"foo";
            expected.fMask = std::get<2>(a);

            ExpectEq_ShellExecuteInfoW(expected, p);

            // reset for the next loop
            *std::get<1>(a) = oldValue;
        }

        // invalid case - passing the argument without a value
        // is a parse error
        // valid case - passing the argument
        // sets the string and the flag
        {
            LPCWSTR argv[] = {
                L"ShellExecuteEx.exe",
                L"--file", L"notepad.exe",
                L"--show", L"SW_NORMAL",
                std::get<0>(a)
            };

            Prefs p;
            bool run = false;
            EXPECT_EQ(false, p.Parse(_countof(argv), argv, run));
            // no expectation on run
        }

        // invalid case - passing the argument twice
        // is a parse error
        // valid case - passing the argument
        // sets the string and the flag
        {
            LPCWSTR argv[] = {
                L"ShellExecuteEx.exe",
                L"--file", L"notepad.exe",
                L"--show", L"SW_NORMAL",
                std::get<0>(a), L"foo",
                std::get<0>(a), L"foo",
            };

            Prefs p;
            bool run = false;
            EXPECT_EQ(false, p.Parse(_countof(argv), argv, run));
            // no expectation on run
        }
    }
}
