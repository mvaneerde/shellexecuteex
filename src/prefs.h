// prefs.h

class Prefs : public SHELLEXECUTEINFOW {
public:
    Prefs(IWindowsApi *api);
    ~Prefs();
    HRESULT Parse(int argc, LPCWSTR argv[]);
    int LogResult(BOOL result, DWORD error);
    bool RelayExitCode();

private:
    IWindowsApi *m_api = nullptr;
    bool m_relayExitCode = false;
};

