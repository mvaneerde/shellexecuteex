// windows-api.h

// mockable interface
class IWindowsApi {
public:
    virtual HRESULT CoInitializeEx(LPVOID reserved, DWORD coinit) = 0;
    virtual BOOL CloseHandle(HANDLE h) = 0;
    virtual HRESULT CLSIDFromString(PCWSTR s, LPCLSID c) = 0;
    virtual void CoTaskMemFree(LPVOID p) = 0;
    virtual void CoUninitialize() = 0;
    virtual void FreeKnownFolderDefinitionFields(KNOWNFOLDER_DEFINITION *d) = 0;
    virtual HWND GetConsoleWindow() = 0;
    virtual BOOL GetExitCodeProcess(HANDLE process, LPDWORD exitCode) = 0;
    virtual DWORD GetLastError() = 0;
    virtual HRESULT SHGetKnownFolderIDList(
        REFKNOWNFOLDERID id,
        DWORD flags,
        HANDLE token,
        PIDLIST_ABSOLUTE *idl
    ) = 0;
    virtual HRESULT SHParseDisplayName(
        PCWSTR name,
        IBindCtx *context,
        PIDLIST_ABSOLUTE *idlist,
        SFGAOF in,
        SFGAOF *out
    ) = 0;
    virtual BOOL ShellExecuteExW(LPSHELLEXECUTEINFOW info) = 0;
    virtual DWORD WaitForSingleObject(HANDLE h, DWORD milliseconds) = 0;
};

// implementation that calls the real Windows APIs
class WindowsApi : public IWindowsApi {
public:
    HRESULT CoInitializeEx(LPVOID reserved, DWORD coinit) override;
    BOOL CloseHandle(HANDLE h) override;
    HRESULT CLSIDFromString(PCWSTR s, LPCLSID c) override;
    void CoTaskMemFree(LPVOID p) override;
    void CoUninitialize() override;
    void FreeKnownFolderDefinitionFields(KNOWNFOLDER_DEFINITION *d) override;
    HWND GetConsoleWindow() override;
    BOOL GetExitCodeProcess(HANDLE process, LPDWORD exitCode) override;
    DWORD GetLastError() override;
    BOOL ShellExecuteExW(LPSHELLEXECUTEINFOW info) override;
    HRESULT SHGetKnownFolderIDList(
        REFKNOWNFOLDERID id,
        DWORD flags,
        HANDLE token,
        PIDLIST_ABSOLUTE *idl
    ) override;
    HRESULT SHParseDisplayName(
        PCWSTR name,
        IBindCtx *context,
        PIDLIST_ABSOLUTE *idlist,
        SFGAOF in,
        SFGAOF *out
    ) override;
    DWORD WaitForSingleObject(HANDLE h, DWORD milliseconds) override;
};
