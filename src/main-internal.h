// main-internal.h

struct MainContext {
    IWindowsApi* api;
    IKnownFolders* knownFolders;
    IUsage* usage;
    IPrefs* prefs;
    IMessagePump* messagePump;
};

int wmain_testable(int argc, LPCWSTR argv[]);
int wmain_mockable(
    int argc,
    LPCWSTR argv[],
    MainContext context
);
