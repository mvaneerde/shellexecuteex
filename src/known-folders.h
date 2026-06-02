// known-folder.h
class IKnownFolders {
public:
    virtual HRESULT GetManager(IKnownFolderManager **manager) = 0;
    virtual HRESULT PrintKnownFolders(IKnownFolderManager *manager) = 0;
    virtual LPCWSTR String_From_KF_CATEGORY(KF_CATEGORY category) = 0;
};

class KnownFolders : public IKnownFolders {
public:
    KnownFolders(IWindowsApi *api);

    // IKnownFolders
    HRESULT GetManager(IKnownFolderManager **manager) override;
    HRESULT PrintKnownFolders(IKnownFolderManager *manager) override;
    LPCWSTR String_From_KF_CATEGORY(KF_CATEGORY category) override;

private:
    IWindowsApi *m_api;
};
