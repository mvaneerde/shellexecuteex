// windows-api.h

// mockable interface
class IWindowsApi {
public:
    virtual HRESULT CoInitializeEx(LPVOID reserved, DWORD coinit) = 0;
    virtual BOOL CloseHandle(HANDLE h) = 0;
    virtual HRESULT CoCreateInstance(
        REFCLSID clsid,
        LPUNKNOWN outer,
        DWORD context,
        REFIID iid,
        LPVOID *out
    ) = 0;
    virtual void CoTaskMemFree(LPVOID p) = 0;
    virtual void CoUninitialize() = 0;
    virtual LRESULT DispatchMessageW(const MSG *msg) = 0;
    virtual BOOL EnumWindows(WNDENUMPROC callback, LPARAM param) = 0;
    virtual BOOL EnumThreadWindows(DWORD threadId, WNDENUMPROC callback, LPARAM param) = 0;
    virtual void FreeKnownFolderDefinitionFields(KNOWNFOLDER_DEFINITION *d) = 0;
    virtual HWND GetConsoleWindow() = 0;
    virtual DWORD GetCurrentProcessId() = 0;
    virtual DWORD GetCurrentThreadId() = 0;
    virtual DWORD GetWindowThreadProcessId(HWND window, LPDWORD processId) = 0;
    virtual BOOL GetExitCodeProcess(HANDLE process, LPDWORD exitCode) = 0;
    virtual DWORD GetLastError() = 0;
    virtual HRESULT IIDFromString(PCWSTR s, LPIID iid) = 0;
    virtual BOOL IsWindowVisible(HWND window) = 0;
    virtual DWORD MsgWaitForMultipleObjectsEx(
        DWORD count,
        const HANDLE *handles,
        DWORD milliseconds,
        DWORD wakeMask,
        DWORD flags
    ) = 0;
    virtual BOOL PeekMessageW(
        LPMSG msg,
        HWND window,
        UINT filterMin,
        UINT filterMax,
        UINT removeMsg
    ) = 0;
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
    virtual BOOL TranslateMessage(const MSG *msg) = 0;
    virtual DWORD WaitForSingleObject(HANDLE h, DWORD milliseconds) = 0;
};

// implementation that calls the real Windows APIs
class WindowsApi : public IWindowsApi {
public:
    HRESULT CoInitializeEx(LPVOID reserved, DWORD coinit) override;
    BOOL CloseHandle(HANDLE h) override;
    HRESULT CoCreateInstance(
        REFCLSID clsid,
        LPUNKNOWN outer,
        DWORD context,
        REFIID iid,
        LPVOID *out
    ) override;
    void CoTaskMemFree(LPVOID p) override;
    void CoUninitialize() override;
    LRESULT DispatchMessageW(const MSG *msg) override;
    BOOL EnumWindows(WNDENUMPROC callback, LPARAM param) override;
    BOOL EnumThreadWindows(DWORD threadId, WNDENUMPROC callback, LPARAM param) override;
    void FreeKnownFolderDefinitionFields(KNOWNFOLDER_DEFINITION *d) override;
    HWND GetConsoleWindow() override;
    DWORD GetCurrentProcessId() override;
    DWORD GetCurrentThreadId() override;
    DWORD GetWindowThreadProcessId(HWND window, LPDWORD processId) override;
    BOOL GetExitCodeProcess(HANDLE process, LPDWORD exitCode) override;
    DWORD GetLastError() override;
    HRESULT IIDFromString(PCWSTR s, LPIID iid) override;
    BOOL IsWindowVisible(HWND window) override;
    DWORD MsgWaitForMultipleObjectsEx(DWORD count, const HANDLE *handles, DWORD milliseconds, DWORD wakeMask, DWORD flags) override;
    BOOL PeekMessageW(LPMSG msg, HWND window, UINT filterMin, UINT filterMax, UINT removeMsg) override;
    BOOL ShellExecuteExW(LPSHELLEXECUTEINFOW info) override;
    BOOL TranslateMessage(const MSG *msg) override;
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
