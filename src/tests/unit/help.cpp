#include "test-common.h"

TEST(Help, UsageTop) {
    // Invoking a usage statement should
    // - not call ShellExecuteExW
    // - return 0
    std::vector<Args> args;

    // all of these sets of arguments should trigger a top-level usage statement
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

    ::testing::NiceMock<MockWindowsApi> api;

    for (auto a : args) {
        EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);
        
        EXPECT_EQ(0, wmain_internal(
            a.argc,
            a.argv,
            &api
        ));
    }
}

TEST(Help, Flags) {
    LPCWSTR args[] = {
        L"shellexecuteex.exe",
        L"help", L"flags"
    };

    ::testing::NiceMock<MockWindowsApi> api;
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);

    EXPECT_EQ(0, wmain_internal(
        _countof(args),
        args,
        &api
    ));
}

TEST(Help, KnownFolders) {
    LPCWSTR args[] = {
        L"shellexecuteex.exe",
        L"help", L"known-folders"
    };

    ::testing::NiceMock<MockWindowsApi> api;
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);

    EXPECT_EQ(0, wmain_internal(
        _countof(args),
        args,
        &api
    ));
}

TEST(Help, ShellExecuteEx) {
    LPCWSTR args[] = {
        L"shellexecuteex.exe",
        L"help", L"shellexecuteex"
    };

    ::testing::NiceMock<MockWindowsApi> api;
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);

    EXPECT_EQ(0, wmain_internal(
        _countof(args),
        args,
        &api
    ));
}

TEST(Help, ShowOptions) {
    LPCWSTR args[] = {
        L"shellexecuteex.exe",
        L"help", L"show-options"
    };

    ::testing::NiceMock<MockWindowsApi> api;
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);

    EXPECT_EQ(0, wmain_internal(
        _countof(args),
        args,
        &api
    ));
}

TEST(Help, Invalid) {
    LPCWSTR args[] = {
        L"shellexecuteex.exe",
        L"help", L"no-such-topic"
    };

    ::testing::NiceMock<MockWindowsApi> api;
    EXPECT_CALL(api, ShellExecuteExW(_)).Times(0);

    EXPECT_NE(0, wmain_internal(
        _countof(args),
        args,
        &api
    ));
}
