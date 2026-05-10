// windows-api.h

// mockable interface
class IWindowsApi {
public:
    virtual HRESULT CoInitializeEx(LPVOID reserved, DWORD coinit) = 0;
    virtual BOOL CloseHandle(HANDLE h) = 0;
    virtual void CoUninitialize() = 0;
    virtual HWND GetConsoleWindow() = 0;
    virtual BOOL GetExitCodeProcess(HANDLE process, LPDWORD exitCode) = 0;
    virtual BOOL ShellExecuteExW(LPSHELLEXECUTEINFOW info) = 0;
    virtual DWORD WaitForSingleObject(HANDLE h, DWORD milliseconds) = 0;
};

// implementation that calls the real Windows APIs
class WindowsApi : public IWindowsApi {
public:
    HRESULT CoInitializeEx(LPVOID reserved, DWORD coinit) override;
    BOOL CloseHandle(HANDLE h) override;
    void CoUninitialize() override;
    HWND GetConsoleWindow() override;
    BOOL GetExitCodeProcess(HANDLE process, LPDWORD exitCode) override;
    BOOL ShellExecuteExW(LPSHELLEXECUTEINFOW info) override;
    DWORD WaitForSingleObject(HANDLE h, DWORD milliseconds) override;
};
