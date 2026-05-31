// known-folder.h
namespace KnownFolders {
    HRESULT GetManager(IWindowsApi *api, IKnownFolderManager **manager);
    HRESULT PrintKnownFolders(IWindowsApi *api, IKnownFolderManager *manager);
    LPCWSTR String_From_KF_CATEGORY(KF_CATEGORY category);
};
