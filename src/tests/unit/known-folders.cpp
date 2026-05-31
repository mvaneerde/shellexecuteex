#include "test-common.h"

class MockKnownFolderManager : public IKnownFolderManager {
public:
    // IUnknown
    MOCK_METHOD(HRESULT, QueryInterface, (REFIID, void **), (override));
    MOCK_METHOD(ULONG, AddRef, (), (override));
    MOCK_METHOD(ULONG, Release, (), (override));

    // IKnownFolderManager
    MOCK_METHOD(HRESULT, RegisterFolder, (REFKNOWNFOLDERID, const KNOWNFOLDER_DEFINITION *), (override));
    MOCK_METHOD(HRESULT, UnregisterFolder, (REFKNOWNFOLDERID), (override));
    MOCK_METHOD(HRESULT, FindFolderFromPath, (LPCWSTR, FFFP_MODE, IKnownFolder **), (override));
    MOCK_METHOD(HRESULT, FindFolderFromIDList, (PCIDLIST_ABSOLUTE, IKnownFolder **), (override));
    MOCK_METHOD(HRESULT, GetFolder, (REFKNOWNFOLDERID, IKnownFolder **), (override));
    MOCK_METHOD(HRESULT, GetFolderIds, (KNOWNFOLDERID **, UINT *), (override));
    MOCK_METHOD(HRESULT, GetFolderByName, (LPCWSTR, IKnownFolder **), (override));
    MOCK_METHOD(HRESULT, FolderIdFromCsidl, (int, KNOWNFOLDERID *), (override));
    MOCK_METHOD(HRESULT, FolderIdToCsidl, (REFKNOWNFOLDERID, int *), (override));
    MOCK_METHOD(HRESULT, Redirect, (REFKNOWNFOLDERID, HWND, KF_REDIRECT_FLAGS, LPCWSTR, UINT, const KNOWNFOLDERID *, LPWSTR *), (override));
};

class MockKnownFolder : public IKnownFolder {
public:
    // IUnknown
    MOCK_METHOD(HRESULT, QueryInterface, (REFIID, void **), (override));
    MOCK_METHOD(ULONG, AddRef, (), (override));
    MOCK_METHOD(ULONG, Release, (), (override));

    // IKnownFolder
    MOCK_METHOD(HRESULT, GetId, (KNOWNFOLDERID *), (override));
    MOCK_METHOD(HRESULT, GetCategory, (KF_CATEGORY *), (override));
    MOCK_METHOD(HRESULT, GetShellItem, (DWORD, REFIID, void **), (override));
    MOCK_METHOD(HRESULT, GetPath, (DWORD, PWSTR *), (override));
    MOCK_METHOD(HRESULT, SetPath, (DWORD, LPCWSTR), (override));
    MOCK_METHOD(HRESULT, GetIDList, (DWORD, PIDLIST_ABSOLUTE *), (override));
    MOCK_METHOD(HRESULT, GetFolderDefinition, (KNOWNFOLDER_DEFINITION *), (override));
    MOCK_METHOD(HRESULT, GetRedirectionCapabilities, (KF_REDIRECTION_CAPABILITIES *), (override));
    MOCK_METHOD(HRESULT, GetFolderType, (FOLDERTYPEID *), (override));
};

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

    IKnownFolderManager *manager = reinterpret_cast<IKnownFolderManager *>(0x1);
    EXPECT_EQ(failure, KnownFolders::GetManager(&mockApi, &manager));
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

    IKnownFolderManager *manager = nullptr;
    EXPECT_EQ(S_OK, KnownFolders::GetManager(&mockApi, &manager));
    EXPECT_EQ(&fakeManager, manager);
}

TEST(KnownFolders, PrintKnownFolders_GetFolderIdsFailure) {
    ::testing::NiceMock<MockWindowsApi> mockApi;
    ::testing::NiceMock<MockKnownFolderManager> mockManager;

    HRESULT failure = -12345;
    EXPECT_CALL(mockManager, GetFolderIds(_, _)).Times(1).WillOnce(Return(failure));

    EXPECT_EQ(failure, KnownFolders::PrintKnownFolders(&mockApi, &mockManager));
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

    EXPECT_EQ(failure, KnownFolders::PrintKnownFolders(&mockApi, &mockManager));
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

    EXPECT_EQ(failure, KnownFolders::PrintKnownFolders(&mockApi, &mockManager));
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

    EXPECT_EQ(S_OK, KnownFolders::PrintKnownFolders(&mockApi, &mockManager));
}

TEST(KnownFolders, KF_CATEGORY_Valid) {
    EXPECT_STREQ(L"KF_CATEGORY_VIRTUAL", KnownFolders::String_From_KF_CATEGORY(KF_CATEGORY_VIRTUAL));
    EXPECT_STREQ(L"KF_CATEGORY_FIXED", KnownFolders::String_From_KF_CATEGORY(KF_CATEGORY_FIXED));
    EXPECT_STREQ(L"KF_CATEGORY_COMMON", KnownFolders::String_From_KF_CATEGORY(KF_CATEGORY_COMMON));
    EXPECT_STREQ(L"KF_CATEGORY_PERUSER", KnownFolders::String_From_KF_CATEGORY(KF_CATEGORY_PERUSER));
}

TEST(KnownFolders, KF_CATEGORY_Invalid) {
    EXPECT_STREQ(L"Unrecognized", KnownFolders::String_From_KF_CATEGORY(KF_CATEGORY(-1)));
}
