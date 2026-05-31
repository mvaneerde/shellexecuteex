#include "test-common.h"

TEST(KnownFolders, GetManagerFailure) {
    ::testing::NiceMock<MockWindowsApi> mockApi;

    HRESULT failure = -12345;
 
    EXPECT_CALL(mockApi, CoCreateInstance(
        CLSID_KnownFolderManager,
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IKnownFolderManager),
        _
    )).Times(1).WillOnce(Invoke([failure](
        REFCLSID clsid,
        LPUNKNOWN outer,
        DWORD context,
        REFIID iid,
        LPVOID *out
    ) -> HRESULT {
        *out = nullptr;
        return failure;
    }));

    KnownFolders knownFolders(&mockApi);
    IKnownFolderManager *manager = reinterpret_cast<IKnownFolderManager *>(0x1);
    EXPECT_EQ(failure, knownFolders.GetManager(&manager));
    EXPECT_EQ(nullptr, manager);
}

TEST(KnownFolders, GetManagerSuccess) {
    ::testing::NiceMock<MockWindowsApi> mockApi;
    ::testing::NiceMock<MockKnownFolderManager> fakeManager;

    EXPECT_CALL(mockApi, CoCreateInstance(
        CLSID_KnownFolderManager,
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IKnownFolderManager),
        _
    )).Times(1).WillOnce(Invoke([&fakeManager](
        REFCLSID clsid,
        LPUNKNOWN outer,
        DWORD context,
        REFIID iid,
        LPVOID *out
    ) -> HRESULT {
        *out = reinterpret_cast<IKnownFolderManager *>(&fakeManager);
        return S_OK;
    }));

    KnownFolders knownFolders(&mockApi);
    IKnownFolderManager *manager = nullptr;
    EXPECT_EQ(S_OK, knownFolders.GetManager(&manager));
    EXPECT_EQ(&fakeManager, manager);
}

TEST(KnownFolders, PrintKnownFolders_GetFolderIdsFailure) {
    ::testing::NiceMock<MockWindowsApi> mockApi;
    ::testing::NiceMock<MockKnownFolderManager> mockManager;

    HRESULT failure = -12345;
    EXPECT_CALL(mockManager, GetFolderIds(_, _)).Times(1).WillOnce(Return(failure));

    KnownFolders knownFolders(&mockApi);
    EXPECT_EQ(failure, knownFolders.PrintKnownFolders(&mockManager));
}

TEST(KnownFolders, PrintKnownFolders_GetFolderFailure) {
    ::testing::NiceMock<MockWindowsApi> mockApi;
    ::testing::NiceMock<MockKnownFolderManager> mockManager;

    HRESULT failure = -12345;
    GUID folderId = {1, 2, 3, {4, 5, 6, 7, 8, 9, 10, 11}};
    GUID folderIds[] = { folderId };
    EXPECT_CALL(mockManager, GetFolderIds(_, _)).Times(1).WillOnce(Invoke(
        [&folderIds](KNOWNFOLDERID **outIds, UINT *outCount) -> HRESULT {
            *outIds = folderIds;
            *outCount = _countof(folderIds);
            return S_OK;
        }
    ));
    EXPECT_CALL(mockManager, GetFolder(folderId, _)).Times(1).WillOnce(Return(failure));

    KnownFolders knownFolders(&mockApi);
    EXPECT_EQ(failure, knownFolders.PrintKnownFolders(&mockManager));
}

TEST(KnownFolders, PrintKnownFolders_GetFolderDefinitionFailure) {
    ::testing::NiceMock<MockWindowsApi> mockApi;
    ::testing::NiceMock<MockKnownFolderManager> mockManager;
    ::testing::NiceMock<MockKnownFolder> mockFolder;

    HRESULT failure = -12345;
    GUID folderId = {1, 2, 3, {4, 5, 6, 7, 8, 9, 10, 11}};
    GUID folderIds[] = { folderId };
    EXPECT_CALL(mockManager, GetFolderIds(_, _)).Times(1).WillOnce(Invoke(
        [&folderIds](KNOWNFOLDERID **outIds, UINT *outCount) -> HRESULT {
            *outIds = folderIds;
            *outCount = _countof(folderIds);
            return S_OK;
        }
    ));
    EXPECT_CALL(mockManager, GetFolder(folderId, _)).Times(1).WillOnce(Invoke(
        [&mockFolder](REFKNOWNFOLDERID id, IKnownFolder **outFolder) -> HRESULT {
            *outFolder = &mockFolder;
            return S_OK;
        }
    ));
    EXPECT_CALL(mockFolder, GetFolderDefinition(_)).Times(1).WillOnce(Return(failure));
    EXPECT_CALL(mockFolder, AddRef()).Times(0);
    EXPECT_CALL(mockFolder, Release()).Times(1);

    KnownFolders knownFolders(&mockApi);
    EXPECT_EQ(failure, knownFolders.PrintKnownFolders(&mockManager));
}

TEST(KnownFolders, PrintKnownFolders_Success) {
    ::testing::NiceMock<MockWindowsApi> mockApi;

    ::testing::NiceMock<MockKnownFolder> mockFolder1;
    GUID folderId1 = {1, 2, 3, {4, 5, 6, 7, 8, 9, 10, 11}};
    KNOWNFOLDER_DEFINITION definition1 = {};

    ::testing::NiceMock<MockKnownFolder> mockFolder2;
    GUID folderId2 = {11, 10, 9, {8, 7, 6, 5, 4, 3, 2, 1}};
    KNOWNFOLDER_DEFINITION definition2 = {};

    ::testing::NiceMock<MockKnownFolderManager> mockManager;
    GUID folderIds[] = { folderId1, folderId2 };

    EXPECT_CALL(mockManager, GetFolderIds(_, _)).Times(1).WillOnce(Invoke(
        [&folderIds](KNOWNFOLDERID **outIds, UINT *outCount) -> HRESULT {
            *outIds = folderIds;
            *outCount = _countof(folderIds);
            return S_OK;
        }
    ));
    EXPECT_CALL(mockManager, GetFolder(folderId1, _)).Times(1).WillOnce(Invoke(
        [&mockFolder1](REFKNOWNFOLDERID id, IKnownFolder **outFolder) -> HRESULT {
            *outFolder = &mockFolder1;
            return S_OK;
        }
    ));
    EXPECT_CALL(mockManager, GetFolder(folderId2, _)).Times(1).WillOnce(Invoke(
        [&mockFolder2](REFKNOWNFOLDERID id, IKnownFolder **outFolder) -> HRESULT {
            *outFolder = &mockFolder2;
            return S_OK;
        }
    ));
    EXPECT_CALL(mockFolder1, AddRef()).Times(0);
    EXPECT_CALL(mockFolder2, AddRef()).Times(0);
    EXPECT_CALL(mockFolder1, Release()).Times(1);
    EXPECT_CALL(mockFolder2, Release()).Times(1);
    EXPECT_CALL(mockFolder1, GetFolderDefinition(_)).Times(1).WillOnce(Invoke(
        [&definition1](KNOWNFOLDER_DEFINITION *outDefinition) -> HRESULT {
            *outDefinition = definition1;
            return S_OK;
        }
    ));
    EXPECT_CALL(mockFolder2, GetFolderDefinition(_)).Times(1).WillOnce(Invoke(
        [definition2](KNOWNFOLDER_DEFINITION *outDefinition) -> HRESULT {
            *outDefinition = definition2;
            return S_OK;
        }
    ));
    EXPECT_CALL(mockApi, FreeKnownFolderDefinitionFields(_)).Times(2);
    EXPECT_CALL(mockApi, CoTaskMemFree(folderIds)).Times(1);

    KnownFolders knownFolders(&mockApi);
    EXPECT_EQ(S_OK, knownFolders.PrintKnownFolders(&mockManager));
}

TEST(KnownFolders, KF_CATEGORY_Valid) {
    ::testing::NiceMock<MockWindowsApi> mockApi;
    KnownFolders knownFolders(&mockApi);

    EXPECT_STREQ(L"KF_CATEGORY_VIRTUAL", knownFolders.String_From_KF_CATEGORY(KF_CATEGORY_VIRTUAL));
    EXPECT_STREQ(L"KF_CATEGORY_FIXED", knownFolders.String_From_KF_CATEGORY(KF_CATEGORY_FIXED));
    EXPECT_STREQ(L"KF_CATEGORY_COMMON", knownFolders.String_From_KF_CATEGORY(KF_CATEGORY_COMMON));
    EXPECT_STREQ(L"KF_CATEGORY_PERUSER", knownFolders.String_From_KF_CATEGORY(KF_CATEGORY_PERUSER));
}

TEST(KnownFolders, KF_CATEGORY_Invalid) {
    ::testing::NiceMock<MockWindowsApi> mockApi;
    KnownFolders knownFolders(&mockApi);

    EXPECT_STREQ(L"Unrecognized", knownFolders.String_From_KF_CATEGORY(KF_CATEGORY(-1)));
}
