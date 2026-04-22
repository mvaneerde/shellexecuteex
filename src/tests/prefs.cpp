#include "test-common.h"

TEST(Prefs, MissingShow) {
    // --show should be required
    LPCWSTR notepad_args[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe"
    };

    Prefs p;
    bool run = true; // Parse should flip this to "false"
    bool result = p.Parse(_countof(notepad_args), notepad_args, run);

    EXPECT_EQ(result, false);
    EXPECT_EQ(run, false);
}

TEST(Prefs, Usage) {
    std::vector<Args> args;

    // all of these sets of arguments should trigger a usage statement
    // with a success return
    // and a "do not run anything" output
    LPCWSTR no_args[] = { L"shellexecuteex.exe" };
    args.push_back(Args(_countof(no_args), no_args));

    LPCWSTR help[][2] = {
        { L"shellexecuteex.exe", L"-?" },
        { L"shellexecuteex.exe", L"/?" },
        { L"shellexecuteex.exe", L"--help" },
    };
    for (int i = 0; i < _countof(help); i++) {
        args.push_back(Args(_countof(help[i]), help[i]));
    }

    for (auto a : args) {
        Prefs p;

        bool run = true; // Parse should flip this to "false"
        bool result = p.Parse(a.argc, a.argv, run);

        EXPECT_EQ(result, true);
        EXPECT_EQ(run, false);
    }
}
