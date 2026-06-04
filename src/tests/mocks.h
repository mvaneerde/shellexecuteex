// mocks.h

class MockWindowsApi : public IWindowsApi {
public:
    MOCK_METHOD(HRESULT, CoInitializeEx, (LPVOID reserved, DWORD coinit), (override));
    MOCK_METHOD(BOOL, CloseHandle, (HANDLE h), (override));
    MOCK_METHOD(HRESULT, CoCreateInstance, (
        REFCLSID clsid,
        LPUNKNOWN outer,
        DWORD context,
        REFIID iid,
        LPVOID *out
    ), (override));
    MOCK_METHOD(void, CoTaskMemFree, (PVOID p), (override));
    MOCK_METHOD(void, CoUninitialize, (), (override));
    MOCK_METHOD(void, FreeKnownFolderDefinitionFields, (KNOWNFOLDER_DEFINITION *d), (override));
    MOCK_METHOD(HWND, GetConsoleWindow, (), (override));
    MOCK_METHOD(BOOL, GetExitCodeProcess, (HANDLE process, LPDWORD exitCode), (override));
    MOCK_METHOD(DWORD, GetLastError, (), (override));
    MOCK_METHOD(HRESULT, IIDFromString, (PCWSTR s, LPIID iid), (override));
    MOCK_METHOD(BOOL, ShellExecuteExW, (LPSHELLEXECUTEINFOW info), (override));
    MOCK_METHOD(HRESULT, SHGetKnownFolderIDList, (
        REFKNOWNFOLDERID id,
        DWORD flags,
        HANDLE token,
        PIDLIST_ABSOLUTE *idl
    ), (override));
    MOCK_METHOD(HRESULT, SHParseDisplayName, (
        PCWSTR name,
        IBindCtx *context,
        PIDLIST_ABSOLUTE *idlist,
        SFGAOF in,
        SFGAOF *out
    ), (override));
    MOCK_METHOD(DWORD, WaitForSingleObject, (HANDLE h, DWORD milliseconds), (override));
};

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

class MockKnownFolders : public IKnownFolders {
public:
    MOCK_METHOD(HRESULT, GetManager, (IKnownFolderManager **), (override));
    MOCK_METHOD(HRESULT, PrintKnownFolders, (IKnownFolderManager *), (override));
    MOCK_METHOD(LPCWSTR, String_From_KF_CATEGORY, (KF_CATEGORY), (override));
};

class MockUsage : public IUsage {
public:
    MOCK_METHOD(HRESULT, HandleUsage, (int, LPCWSTR[], bool &), (override));
    MOCK_METHOD(HRESULT, HandleHelp, (int, LPCWSTR[], bool &), (override));
};

class MockPrefs : public IPrefs {
public:
    MOCK_METHOD(HRESULT, Parse, (int, LPCWSTR[]), (override));
    MOCK_METHOD(bool, RelayExitCode, (), (override));
    MOCK_METHOD(LPSHELLEXECUTEINFOW, GetShellExecuteInfo, (), (override));
    MOCK_METHOD(bool, WaitForWindow, (), (override));
};
