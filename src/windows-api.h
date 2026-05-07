// windows-api.h

// mockable interface
class IWindowsApi {
public:
    virtual BOOL CloseHandle(HANDLE h) = 0;
    virtual HWND GetConsoleWindow() = 0;
    virtual BOOL ShellExecuteEx(LPSHELLEXECUTEINFO info) = 0;
};

// implementation that calls the real Windows APIs
class WindowsApi : public IWindowsApi {
    BOOL CloseHandle(HANDLE h);
    HWND GetConsoleWindow();
    BOOL ShellExecuteEx(LPSHELLEXECUTEINFO info);
};
