// cleanup.h

class CoTaskMemFreeOnExit {
public:
    CoTaskMemFreeOnExit(IWindowsApi *a, PVOID m);
    ~CoTaskMemFreeOnExit();
private:
    IWindowsApi *api;
    PVOID memory;
};

class CoUninitializeOnExit {
public:
    CoUninitializeOnExit(IWindowsApi *a);
    ~CoUninitializeOnExit();

private:
    IWindowsApi *api;
};

class FreeKnownFolderDefinitionFieldsOnExit {
public:
    FreeKnownFolderDefinitionFieldsOnExit(IWindowsApi *a, KNOWNFOLDER_DEFINITION *d);
    ~FreeKnownFolderDefinitionFieldsOnExit();
private:
    IWindowsApi *api;
    KNOWNFOLDER_DEFINITION *definition;
};

class ReleaseOnExit {
public:
    ReleaseOnExit(IUnknown *u);
    ~ReleaseOnExit();

private:
    IUnknown *unknown;
};