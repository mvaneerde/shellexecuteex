// cleanup.h

class CoTaskMemFreeOnExit {
public:
    CoTaskMemFreeOnExit(PVOID m);
    ~CoTaskMemFreeOnExit();
private:
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
    FreeKnownFolderDefinitionFieldsOnExit(KNOWNFOLDER_DEFINITION *d);
    ~FreeKnownFolderDefinitionFieldsOnExit();
private:
    KNOWNFOLDER_DEFINITION *definition;
};

class ReleaseOnExit {
public:
    ReleaseOnExit(IUnknown *u);
    ~ReleaseOnExit();

private:
    IUnknown *unknown;
};