#include "test-common.h"

TEST(Usage, Top) {
    ::testing::NiceMock<MockWindowsApi> api;
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
        Usage usage(&api);

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

TEST(Usage, NotUsage) {
    ::testing::NiceMock<MockWindowsApi> api;

    LPCWSTR not_usage[] = {
        L"shellexecuteex.exe",
        L"non-existent-argument"
    };

    Usage usage(&api);
    bool handled = true; // HandleUsage should flip this to "false"
    EXPECT_EQ(S_OK, usage.HandleUsage(_countof(not_usage), not_usage, handled));
    EXPECT_EQ(handled, true);
}

TEST(Usage, Help_SimpleTopics) {
    ::testing::NiceMock<MockWindowsApi> api;

    // each of these should trigger a help statement
    LPCWSTR topics[][3] = {
        { L"shellexecuteex.exe", L"help", L"flags" },
        // known-folders is more complex
        { L"shellexecuteex.exe", L"help", L"shellexecuteex" },
        { L"shellexecuteex.exe", L"help", L"show-options" },
    };

    for (int i = 0; i < _countof(topics); i++) {
        Usage usage(&api);

        std::wstring context = std::wstring(L"topic=") + std::wstring(topics[i][2]);
        SCOPED_TRACE(context);

        bool handled = false; // HandleHelp should flip this to "true"
        EXPECT_EQ(S_OK, usage.HandleHelp(_countof(topics[i]), topics[i], handled));
        EXPECT_EQ(handled, true);
    }
}

TEST(Usage, Help_KnownFolders_Failure) { 
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolderManager> mockManager;

    EXPECT_CALL(api, CoCreateInstance(CLSID_KnownFolderManager, _, _, _, _)).Times(1).WillOnce(Invoke(
        [&mockManager](
            REFCLSID clsid,
            LPUNKNOWN outer,
            DWORD context,
            REFIID iid,
            LPVOID *out
        ) -> HRESULT {
            *out = reinterpret_cast<IKnownFolderManager *>(&mockManager);
            return S_OK;
        }));

    EXPECT_CALL(mockManager, GetFolderIds(_, _)).Times(1).WillOnce(Invoke(
        [](KNOWNFOLDERID **outIds, UINT *outCount) -> HRESULT {
            *outIds = nullptr;
            *outCount = 0;
            return S_OK;
        }
    ));

    Usage usage(&api);

    LPCWSTR argv[] = { L"shellexecuteex.exe", L"help", L"known-folders" };
    bool handled = false; // HandleHelp should flip this to "true"
    EXPECT_EQ(S_OK, usage.HandleHelp(_countof(argv), argv, handled));
    EXPECT_EQ(handled, true);
}

TEST(Usage, Help_KnownFolders_Success) { 
    ::testing::NiceMock<MockWindowsApi> api;
    ::testing::NiceMock<MockKnownFolderManager> mockManager;

    EXPECT_CALL(api, CoCreateInstance(CLSID_KnownFolderManager, _, _, _, _)).Times(1).WillOnce(Invoke(
        [&mockManager](
            REFCLSID clsid,
            LPUNKNOWN outer,
            DWORD context,
            REFIID iid,
            LPVOID *out
        ) -> HRESULT {
            *out = reinterpret_cast<IKnownFolderManager *>(&mockManager);
            return S_OK;
        }));

    EXPECT_CALL(mockManager, GetFolderIds(_, _)).Times(1).WillOnce(Invoke(
        [](KNOWNFOLDERID **outIds, UINT *outCount) -> HRESULT {
            *outIds = nullptr;
            *outCount = 0;
            return S_OK;
        }
    ));

    Usage usage(&api);

    LPCWSTR argv[] = { L"shellexecuteex.exe", L"help", L"known-folders" };
    bool handled = false; // HandleHelp should flip this to "true"
    EXPECT_EQ(S_OK, usage.HandleHelp(_countof(argv), argv, handled));
    EXPECT_EQ(handled, true);
}

TEST(Usage, Help_NoTopic) {
    ::testing::NiceMock<MockWindowsApi> api;

    // it is invalid to ask for help on no topic
    LPCWSTR no_topic[] = { L"shellexecuteex.exe", L"help" };

    Usage usage(&api);
    bool handled = false;
    EXPECT_EQ(E_INVALIDARG, usage.HandleHelp(_countof(no_topic), no_topic, handled));
    // no expectation on handled
}

TEST(Usage, Help_InvalidTopic) {
    ::testing::NiceMock<MockWindowsApi> api;

    LPCWSTR invalid_topic[] = { L"shellexecuteex.exe", L"help", L"no-such-topic" };

    Usage usage(&api);
    bool handled = false;
    EXPECT_EQ(E_INVALIDARG, usage.HandleHelp(_countof(invalid_topic), invalid_topic, handled));
    // no expectation on handled
}

TEST(Usage, Help_ExtraArguments) {
    ::testing::NiceMock<MockWindowsApi> api;

    // it is invalid to ask for help on a topic with extra arguments
    LPCWSTR extra_args[] = { L"shellexecuteex.exe", L"help", L"flags", L"extra-arg" };

    Usage usage(&api);
    bool handled = false;
    EXPECT_EQ(E_INVALIDARG, usage.HandleHelp(_countof(extra_args), extra_args, handled));
    // no expectation on handled
}