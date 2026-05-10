// prefs.h

class Prefs : public SHELLEXECUTEINFOW {
public:
    Prefs(IWindowsApi *api);
    ~Prefs();
    bool Parse(int argc, LPCWSTR argv[], bool &run);
    void LogResult(BOOL result, DWORD error);
    bool RelayExitCode();

private:
    IWindowsApi *m_api = nullptr;
    bool m_relayExitCode = false;

    static bool IsUsage(int argc, LPCWSTR argv[]);
    static void ShowUsage();
    static int ShowInt_From_String(LPCWSTR s, bool &found);

    struct ShowInt_Mapping {
        ShowInt_Mapping(LPCWSTR n, int v);
        LPCWSTR name;
        int value;
    };

    static ShowInt_Mapping showInts[14];
};

