#include "common.h"

Prefs::Prefs(IWindowsApi *api) :
    SHELLEXECUTEINFOW { sizeof(SHELLEXECUTEINFOW) },
    m_api(api)
{
    hwnd = m_api->GetConsoleWindow();
    nShow = SW_NORMAL;
}

Prefs::~Prefs() {
    if (hProcess != nullptr) {
        m_api->CloseHandle(hProcess);
    }

    if (lpIDList != nullptr) {
        m_api->CoTaskMemFree(lpIDList);
    }
}

HRESULT Prefs::Parse(int argc, LPCWSTR argv[]) {
    // some arguments just set a flag
    class FlagArg {
    public:
        ULONG flag;
        bool seen;

        FlagArg(ULONG f) : flag(f), seen(false) {}
    };

    struct StringCompare {
        bool operator()(LPCWSTR a, LPCWSTR b) const {
            return _wcsicmp(a, b) < 0;
        }
    };

    std::map<LPCWSTR, FlagArg, StringCompare> flagArgs = {
        { L"--async-ok", FlagArg(SEE_MASK_ASYNCOK) },
        { L"--connect-network-drive", FlagArg(SEE_MASK_CONNECTNETDRV) },
        { L"--do-environment-substitution", FlagArg(SEE_MASK_DOENVSUBST) },
        { L"--log-usage", FlagArg(SEE_MASK_FLAG_LOG_USAGE) },
        { L"--no-async", FlagArg(SEE_MASK_NOASYNC) },
        { L"--no-close-process", FlagArg(SEE_MASK_NOCLOSEPROCESS) },
        { L"--no-console", FlagArg(SEE_MASK_NO_CONSOLE) },
        { L"--no-query-class-store", FlagArg(SEE_MASK_NOQUERYCLASSSTORE) },
        { L"--no-ui", FlagArg(SEE_MASK_FLAG_NO_UI) },
        { L"--no-zone-checks", FlagArg(SEE_MASK_NOZONECHECKS) },
        { L"--unicode", FlagArg(SEE_MASK_UNICODE) },
        { L"--wait-for-input-idle", FlagArg(SEE_MASK_WAITFORINPUTIDLE) },
    };

    // some arguments just set a string
    class StringArg {
    public:
        LPCWSTR *setting;
        bool seen;

        StringArg(LPCWSTR *s) : setting(s), seen(false) {}
    };
    
    std::map<LPCWSTR, StringArg, StringCompare> stringArgs = {
        { L"--directory", StringArg(&lpDirectory) },
        { L"--file", StringArg(&lpFile) },
        { L"--parameters", StringArg(&lpParameters) },
        { L"--verb", StringArg(&lpVerb) },
    };

    // mask options
    // SEE_MASK_CLASSKEY TODO
    // SEE_MASK_IDLIST TODO
    // SEE_MASK_INVOKEIDLIST TODO
    // SEE_MASK_ICON TODO
    // SEE_MASK_HOTKEY TODO
    // SEE_MASK_HMONITOR TODO
    // SEE_MASK_FLAG_HINST_IS_SITE TODO

    bool seenClass = false;
    bool seenItemIdListFromDisplayName = false;
    bool seenItemIdListFromKnownFolderId = false;
    bool seenRelayExitCode = false;
    bool seenShow = false;

    for (int i = 1; i < argc; i++) {
        // flag arguments
        {
            auto it = flagArgs.find(argv[i]);
            if (it != flagArgs.end()) {
                FlagArg &arg = it->second;
                if (arg.seen) {
                    LOG(L"Multiple %s arguments passed", it->first);
                    return E_INVALIDARG;
                }
                arg.seen = true;
                fMask |= arg.flag;
                continue;
            }
        }

        // string arguments
        {
            auto it = stringArgs.find(argv[i]);
            if (it != stringArgs.end()) {
                StringArg &arg = it->second;
                if (arg.seen) {
                    LOG(L"Multiple %s arguments passed", it->first);
                    return E_INVALIDARG;
                }
                arg.seen = true;

                if (++i == argc) {
                    LOG(L"%s requires a value", it->first);
                    return E_INVALIDARG;
                }

                *arg.setting = argv[i];
                continue;
            }
        }

        // --class-name
        if (0 == _wcsicmp(argv[i], L"--class-name")) {
            if (seenClass) {
                LOG(L"%s", L"Multiple class arguments passed");
                return E_INVALIDARG;
            }
            seenClass = true;

            i++;
            if (i == argc) {
                LOG(L"%s", L"--class-name requires a value");
                return E_INVALIDARG;
            }

            fMask |= SEE_MASK_CLASSNAME;
            lpClass = argv[i];
            continue;
        }

        // --relay-exit-code
        if (0 == _wcsicmp(argv[i], L"--relay-exit-code")) {
            if (seenRelayExitCode) {
                LOG(L"%s", L"Multiple --relay-exit-code arguments passed");
                return E_INVALIDARG;
            }                
            seenRelayExitCode = true;
            
            m_relayExitCode = true;
            continue;
        }

        // --item-id-list-from-display-name
        if (0 == _wcsicmp(argv[i], L"--item-id-list-from-display-name")) {
            if (seenItemIdListFromDisplayName) {
                LOG(L"%s", L"Multiple --item-id-list-from-display-name arguments passed");
                return E_INVALIDARG;
            }
            seenItemIdListFromDisplayName = true;

            i++;
            if (i == argc) {
                LOG(L"%s", L"--item-id-list-from-display-name requires a value");
                return E_INVALIDARG;
            }

            fMask |= SEE_MASK_IDLIST;

            HRESULT hr = m_api->SHParseDisplayName(
                argv[i],
                nullptr,
                reinterpret_cast<LPITEMIDLIST*>(&lpIDList),
                0,
                nullptr);
            if (FAILED(hr)) { 
                LOG(L"SHParseDisplayName failed: 0x%08x", hr);
                return hr;
            }
            continue;
        }

        // --item-id-list-from-known-folder-id
        if (0 == _wcsicmp(argv[i], L"--item-id-list-from-known-folder-id")) {
            if (seenItemIdListFromKnownFolderId) {
                LOG(L"%s", L"Multiple --item-id-list-from-known-folder-id arguments passed");
                return E_INVALIDARG;
            }
            seenItemIdListFromKnownFolderId = true;

            i++;
            if (i == argc) {
                LOG(L"%s", L"--item-id-list-from-known-folder-id requires a value");
                return E_INVALIDARG;
            }

            fMask |= SEE_MASK_IDLIST;

            GUID clsid = {};
            HRESULT hr = m_api->IIDFromString(argv[i], &clsid);
            if (FAILED(hr)) {
                LOG(L"IIDFromString failed: 0x%08x", hr);
                return hr;
            }

            hr = m_api->SHGetKnownFolderIDList(
                clsid,
                0,
                nullptr,
                reinterpret_cast<LPITEMIDLIST*>(&lpIDList)
            );
            if (FAILED(hr)) {
                LOG(L"SHGetKnownFolderIDList failed: 0x%08x", hr);
                return hr;
            }
            continue;
        }

        // --show
        if (0 == _wcsicmp(argv[i], L"--show")) {
            if (seenShow) {
                LOG(L"%s", L"Multiple --show arguments passed");
                return E_INVALIDARG;
            }
            seenShow = true;

            i++;
            if (i == argc) {
                LOG(L"%s", L"--show requires a value");
                return E_INVALIDARG;
            }

            HRESULT hr = ShowWindowCommand::ParseShowWindowCommand(argv[i], nShow);
            if (FAILED(hr)) {
                // ShowWindowCommand::ParseShowWindowCommand logs failure
                return hr;
            }
            continue;
        }

        // any other argument
        LOG(L"Unrecognized argument %s", argv[i]);
        return E_INVALIDARG;
    }

    if (1 !=
        (stringArgs.at(L"--file").seen ? 1 : 0) +
        (seenItemIdListFromDisplayName ? 1 : 0) +
        (seenItemIdListFromKnownFolderId ? 1 : 0)) {
        LOG(L"%s", L"Specify precisely one of --file, --item-id-list-from-display-name, or --item-id-list-from-known-folder-id");
        return E_INVALIDARG;
    }

    // --relay-exit-code requires --no-close-process
    if (seenRelayExitCode &&
        !flagArgs.at(L"--no-close-process").seen) {
        LOG(L"%s", L"--relay-exit-code requires --no-close-process");
        return E_INVALIDARG;
    }

    return S_OK;
}

int Prefs::LogResult(BOOL result, DWORD error) {
    LOG(L"ShellExecuteExW %s", (result ? L"succeeded" : L"failed"));
    if (result) {
        LOG(L"hProcess: 0x%p", hProcess);
    } else {
        LOG(L"Last error: %d", error);
    }
    LOG(L"hInstApp: 0x%p", hInstApp);
    
    if (result == FALSE && error == ERROR_SUCCESS) {
        LOG(L"%s", L"ShellExecuteExW failed but no error was set");
        return ERROR_INTERNAL_ERROR;
    } else {
        return error;
    }
}

bool Prefs::RelayExitCode() {
    return m_relayExitCode;
}
