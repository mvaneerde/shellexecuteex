#include "test-common.h"

// String arguments
TEST(Prefs, StringArguments) {
    SHELLEXECUTEINFOW expected = { sizeof(expected) };
    expected.hwnd = GetConsoleWindow();
    expected.lpFile = L"notepad.exe";
    expected.nShow = SW_NORMAL;

    struct StringArg {
        LPCWSTR name;
        LPCWSTR *setting;
        DWORD flag;
    };
    
    StringArg stringArgs[] = {
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

    WindowsApi api;
    for (auto arg : stringArgs) {
        // valid case - passing the argument
        // sets the string and the flag
        {
            std::wstring context = std::wstring(L"arg=") + std::wstring(arg.name);
            SCOPED_TRACE(context);

            LPCWSTR argv[] = {
                L"ShellExecuteEx.exe",
                L"--file", L"notepad.exe",
                arg.name, L"foo"
            };

            Prefs p(&api);
            EXPECT_EQ(S_OK, p.Parse(_countof(argv), argv));

            LPCWSTR oldValue = *arg.setting;

            *arg.setting = L"foo";
            expected.fMask = arg.flag;

            LPSHELLEXECUTEINFO actual = p.GetShellExecuteInfo();

            ExpectEq_ShellExecuteInfoW(expected, *actual);

            // reset for the next loop
            *arg.setting = oldValue;
        }

        // invalid case - passing the argument without a value
        // is a parse error
        // valid case - passing the argument
        // sets the string and the flag
        {
            LPCWSTR argv[] = {
                L"ShellExecuteEx.exe",
                L"--file", L"notepad.exe",
                arg.name
            };

            Prefs p(&api);
            EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(argv), argv));
        }

        // invalid case - passing the argument twice
        // is a parse error
        // valid case - passing the argument
        // sets the string and the flag
        {
            LPCWSTR argv[] = {
                L"ShellExecuteEx.exe",
                L"--file", L"notepad.exe",
                arg.name, L"foo",
                arg.name, L"foo",
            };

            Prefs p(&api);
            EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(argv), argv));
        }
    }
}

// --file is special
TEST(Prefs, File) {
    WindowsApi api;

    // valid case - passing the argument
    {
        SHELLEXECUTEINFOW expected = { sizeof(expected) };
        expected.hwnd = GetConsoleWindow();
        expected.lpFile = L"notepad.exe";
        expected.nShow = SW_NORMAL;

        LPCWSTR argv[] = {
            L"ShellExecuteEx.exe",
            L"--file", L"notepad.exe",
        };

        Prefs p(&api);
        EXPECT_EQ(S_OK, p.Parse(_countof(argv), argv));

        LPSHELLEXECUTEINFO actual = p.GetShellExecuteInfo();

        ExpectEq_ShellExecuteInfoW(expected, *actual);
    }

    // invalid case - --file missing
    {
        LPCWSTR argv[] = {
            L"ShellExecuteEx.exe",
            L"--unicode" // need an argument or we get a usage statement
        };

        Prefs p(&api);
        EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(argv), argv));
    }


    // invalid case - --file present but missing value
    {
        LPCWSTR argv[] = {
            L"ShellExecuteEx.exe",
            L"--file"
        };

        Prefs p(&api);
        EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(argv), argv));
    }

    // invalid case - double --file
    {
        LPCWSTR argv[] = {
            L"ShellExecuteEx.exe",
            L"--file", L"calc.exe",
            L"--file", L"calc.exe",
        };

        Prefs p(&api);
        EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(argv), argv));
    }
}
