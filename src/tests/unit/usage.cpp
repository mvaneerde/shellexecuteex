#include "test-common.h"

TEST(Usage, Top) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    Usage usage(&knownFolders, &api);
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
        std::wstring context =
            std::wstring(L"argc=") + std::to_wstring(a.argc) + std::wstring(L"; ") +
            std::wstring(L"argv=");
        for (int i = 0; i < a.argc; i++) {
            context += std::wstring((i == 0 ? L"" : L" ")) + std::wstring(a.argv[i]);
        }
        SCOPED_TRACE(context);

        bool handled = false; // HandleUsage should flip this to "true"
        EXPECT_EQ(S_OK, usage.HandleUsage(a.argc, a.argv, handled));
        EXPECT_EQ(handled, true);
    }
}

TEST(Usage, NotUsageOrHelp) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    Usage usage(&knownFolders, &api);

    LPCWSTR not_usage_or_help[] = {
        L"shellexecuteex.exe",
        L"non-existent-argument"
    };

    bool handled = true; // HandleUsage should flip this to "false"
    EXPECT_EQ(S_OK, usage.HandleUsage(_countof(not_usage_or_help), not_usage_or_help, handled));
    EXPECT_EQ(handled, false);

    handled = true; // HandleHelp should flip this to "false"
    EXPECT_EQ(S_OK, usage.HandleHelp(_countof(not_usage_or_help), not_usage_or_help, handled));
    EXPECT_EQ(handled, false);

}

TEST(Usage, Help_SimpleTopics) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    Usage usage(&knownFolders, &api);

    // each of these should trigger a help statement
    LPCWSTR topics[][3] = {
        { L"shellexecuteex.exe", L"help", L"flags" },
        // known-folders is more complex
        { L"shellexecuteex.exe", L"help", L"shellexecuteex" },
        { L"shellexecuteex.exe", L"help", L"show-options" },
    };

    for (int i = 0; i < _countof(topics); i++) {
        std::wstring context = std::wstring(L"topic=") + std::wstring(topics[i][2]);
        SCOPED_TRACE(context);

        bool handled = false; // HandleHelp should flip this to "true"
        EXPECT_EQ(S_OK, usage.HandleHelp(_countof(topics[i]), topics[i], handled));
        EXPECT_EQ(handled, true);
    }
}

TEST(Usage, Help_KnownFolders_GetManager_Failure) { 
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    Usage usage(&knownFolders, &api);

    HRESULT failure = -12345;
    EXPECT_CALL(knownFolders, GetManager(_)).Times(1).WillOnce(Return(failure));
    EXPECT_CALL(knownFolders, PrintKnownFolders(_)).Times(0);
    EXPECT_CALL(knownFolders, String_From_KF_CATEGORY(_)).Times(0);

    LPCWSTR argv[] = { L"shellexecuteex.exe", L"help", L"known-folders" };
    bool handled = false;
    EXPECT_EQ(failure, usage.HandleHelp(_countof(argv), argv, handled));
    // no expectation on handled
}

TEST(Usage, Help_KnownFolders_PrintKnownFolders_Failure) { 
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockKnownFolderManager> mockManager;
    Usage usage(&knownFolders, &api);

    EXPECT_CALL(knownFolders, GetManager(_)).Times(1).WillOnce(Invoke(
        [&mockManager](
            IKnownFolderManager **manager
        ) -> HRESULT {
            *manager = &mockManager;
            return S_OK;
        }
    ));

    HRESULT failure = -12345;
    EXPECT_CALL(knownFolders, PrintKnownFolders(_)).Times(1).WillOnce(Return(failure));

    EXPECT_CALL(knownFolders, String_From_KF_CATEGORY(KF_CATEGORY_PERUSER)).Times(0);
    EXPECT_CALL(mockManager, Release()).Times(1);

    LPCWSTR argv[] = { L"shellexecuteex.exe", L"help", L"known-folders" };
    bool handled = false;
    EXPECT_EQ(failure, usage.HandleHelp(_countof(argv), argv, handled));
    // no expectation on handled
}

TEST(Usage, Help_KnownFolders_Success) { 
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    ::testing::NiceMock<MockKnownFolderManager> mockManager;
    Usage usage(&knownFolders, &api);

    EXPECT_CALL(knownFolders, GetManager(_)).Times(1).WillOnce(Invoke(
        [&mockManager](
            IKnownFolderManager **manager
        ) -> HRESULT {
            *manager = &mockManager;
            return S_OK;
        }
    ));

    EXPECT_CALL(knownFolders, PrintKnownFolders(_)).Times(1).WillOnce(Return(S_OK));
    EXPECT_CALL(knownFolders, String_From_KF_CATEGORY(KF_CATEGORY_PERUSER)).Times(0);
    EXPECT_CALL(mockManager, Release()).Times(1);

    LPCWSTR argv[] = { L"shellexecuteex.exe", L"help", L"known-folders" };
    bool handled = false;
    EXPECT_EQ(S_OK, usage.HandleHelp(_countof(argv), argv, handled));
    EXPECT_EQ(handled, true);
}

TEST(Usage, Help_NoTopic) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    Usage usage(&knownFolders, &api);

    // it is invalid to ask for help on no topic
    LPCWSTR no_topic[] = { L"shellexecuteex.exe", L"help" };

    bool handled = false;
    EXPECT_EQ(E_INVALIDARG, usage.HandleHelp(_countof(no_topic), no_topic, handled));
    // no expectation on handled
}

TEST(Usage, Help_InvalidTopic) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    Usage usage(&knownFolders, &api);

    LPCWSTR invalid_topic[] = { L"shellexecuteex.exe", L"help", L"no-such-topic" };

    bool handled = false;
    EXPECT_EQ(E_INVALIDARG, usage.HandleHelp(_countof(invalid_topic), invalid_topic, handled));
    // no expectation on handled
}

TEST(Usage, Help_ExtraArguments) {
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolders> knownFolders;
    Usage usage(&knownFolders, &api);

    // it is invalid to ask for help on a topic with extra arguments
    LPCWSTR extra_args[] = { L"shellexecuteex.exe", L"help", L"flags", L"extra-arg" };

    bool handled = false;
    EXPECT_EQ(E_INVALIDARG, usage.HandleHelp(_countof(extra_args), extra_args, handled));
    // no expectation on handled
}
