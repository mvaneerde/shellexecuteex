// prefs.h

class IPrefs {
public:
    virtual HRESULT Parse(int argc, LPCWSTR argv[]) = 0;
    virtual SHELLEXECUTEINFOW *GetShellExecuteInfo() = 0;
    virtual bool RelayExitCode() = 0;
    virtual bool WaitForWindow() = 0;
};

class Prefs : public IPrefs {
public:
    Prefs(IWindowsApi *api);
    ~Prefs();

    // IPrefs
    HRESULT Parse(int argc, LPCWSTR argv[]) override;
    bool RelayExitCode() override;
    LPSHELLEXECUTEINFOW GetShellExecuteInfo() override;
    bool WaitForWindow() override;

private:
    IWindowsApi *m_api;
    bool m_relayExitCode;
    bool m_waitForWindow;
    SHELLEXECUTEINFOW info;
};

