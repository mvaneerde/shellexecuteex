#include "common.h"

HRESULT KnownFolders::GetManager(IWindowsApi *api,IKnownFolderManager **manager) {
    HRESULT hr = api->CoCreateInstance(
        CLSID_KnownFolderManager,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(manager)
    );

    if (FAILED(hr)) {
        LOG(L"CoCreateInstance(CLSID_KnownFolderManager) failed: 0x%08x", hr);
        return hr;
    }

    return S_OK;
}

HRESULT KnownFolders::PrintKnownFolders(IWindowsApi *api, IKnownFolderManager *manager) {
    KNOWNFOLDERID *folders = nullptr;
    UINT count = 0;
    HRESULT hr = manager->GetFolderIds(&folders, &count);
    if (FAILED(hr)) {
        LOG(L"IKnownFolderManager::GetFolderIds failed: 0x%08x", hr);
        return hr;
    }
    CoTaskMemFreeOnExit freeFolders(api, folders);
    
    LOG(L"Known folders: %u", count);
    for (UINT i = 0; i < count; i++) {
        LOG(L"#%u: " GUID_PRINTF_FORMAT, i + 1, GUID_PRINTF_ARGS(folders[i]));

        IKnownFolder *folder = nullptr;
        hr = manager->GetFolder(folders[i], &folder);
        if (FAILED(hr)) {
            LOG(L"IKnownFolderManager::GetFolder failed: 0x%08x", hr);
            return hr;
        }
        ReleaseOnExit releaseFolder(folder);

        KNOWNFOLDER_DEFINITION definition = {};
        hr = folder->GetFolderDefinition(&definition);
        if (FAILED(hr)) {
            LOG(L"IKnownFolder::GetFolderDefinition failed: 0x%08x", hr);
            return hr;
        }

        FreeKnownFolderDefinitionFieldsOnExit freeDefinition(api, &definition);
        LOG(L"    category: %s (%u)", KnownFolders::String_From_KF_CATEGORY(definition.category), definition.category);
        LOG(L"    pszName: %s", definition.pszName);
        LOG(L"    pszDescription: %s", definition.pszDescription);
        LOG(L"    fidParent: " GUID_PRINTF_FORMAT, GUID_PRINTF_ARGS(definition.fidParent));
        LOG(L"    pszRelativePath: %s", definition.pszRelativePath);
        LOG(L"    pszParsingName: %s", definition.pszParsingName);
        LOG(L"    pszTooltip: %s", definition.pszTooltip);
        LOG(L"    pszLocalizedName: %s", definition.pszLocalizedName);
        LOG(L"    pszIcon: %s", definition.pszIcon);
        LOG(L"    pszSecurity: %s", definition.pszSecurity);
        LOG(L"    dwAttributes: 0x%08x", definition.dwAttributes);
        LOG(L"    kfdFlags: 0x%08x", definition.kfdFlags);
        LOG(L"    ftidType: " GUID_PRINTF_FORMAT, GUID_PRINTF_ARGS(definition.ftidType));
    } 

    return S_OK;
}

#define CASE_RETURN_KF_CATEGORY(c) case c: return L ## #c

LPCWSTR KnownFolders::String_From_KF_CATEGORY(KF_CATEGORY c) {
    switch (c) {
        CASE_RETURN_KF_CATEGORY(KF_CATEGORY_VIRTUAL);
        CASE_RETURN_KF_CATEGORY(KF_CATEGORY_FIXED);
        CASE_RETURN_KF_CATEGORY(KF_CATEGORY_COMMON);
        CASE_RETURN_KF_CATEGORY(KF_CATEGORY_PERUSER);
        default:
            return L"Unrecognized";
    }
}
