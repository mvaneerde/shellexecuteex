// usage.h

class IUsage {
public:
    virtual HRESULT HandleUsage(int argc, LPCWSTR argv[], bool &handled) = 0;
    virtual HRESULT HandleHelp(int argc, LPCWSTR argv[], bool &handled) = 0;
};

struct UsageContext {
    IKnownFolders* knownFolders;
    IWindowsApi* api;
};

class Usage : public IUsage {
public:
    Usage(UsageContext context);

    // IUsage
    HRESULT HandleUsage(int argc, LPCWSTR argv[], bool &handled) override;
    HRESULT HandleHelp(int argc, LPCWSTR argv[], bool &handled) override;

private:
    IKnownFolders *m_knownFolders;
    IWindowsApi *m_api;

    bool IsUsage(int argc, LPCWSTR argv[]);
    void ShowUsage();

    HRESULT IsHelp(int argc, LPCWSTR argv[], bool &help, LPCWSTR &topic);
    HRESULT ShowHelp(LPCWSTR topic);
    HRESULT ShowHelpKnownFolders();
};
