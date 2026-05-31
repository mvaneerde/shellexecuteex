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

HRESULT Prefs::Parse(int argc, LPCWSTR argv[], bool &run) {
    // see if this is a request for a usage statement
    if (IsTopUsage(argc, argv)) {
        ShowTopUsage();
        run = false;
        return S_OK;
    } else if (IsHelpRequest(argc, argv)) {
        run = false;
        return FulfillHelpRequest(argc, argv);
    } else {
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

                bool found = false;
                nShow = ShowInt_From_String(argv[i], found);
                if (!found) {
                    LOG(L"%s", L"Unrecognized value for --show");
                    return E_INVALIDARG;
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

        run = true;
        return S_OK;
    }
}

bool Prefs::IsTopUsage(int argc, LPCWSTR argv[]) {
    // no parameters
    if (argc == 0 + 1) {
        return true;
    }

    // one parameter, -? or equivalent
    if (argc == 1 + 1) {
        LPCWSTR help[] = {
            L"-?",
            L"/?",
            L"--help"
        };

        for (int i = 0; i < _countof(help); i++) {
            if (0 == _wcsicmp(argv[1], help[i])) {
                return true;
            }
        }
    }

    return false;
}

bool Prefs::IsHelpRequest(int argc, LPCWSTR argv[]) {
    // first parameter is "help"
    return (argc >= 1 + 1 ) &&
        (0 == _wcsicmp(argv[1], L"help"));
}

HRESULT Prefs::FulfillHelpRequest(int argc, LPCWSTR argv[]) {
    if (argc == 2 + 1) {
        LPCWSTR topic = argv[2];
        if (0 == _wcsicmp(topic, L"flags")) {
            LOG(L"%s", L"<flags>");
            LOG(L"%s", L"    --async-ok");
            LOG(L"%s", L"        sets SEE_MASK_ASYNCOK");
            LOG(L"%s", L"    --do-environment-substitution");
            LOG(L"%s", L"        sets SEE_MASK_DOENVSUBST");
            LOG(L"%s", L"    --connect-network-drive");
            LOG(L"%s", L"        sets SEE_MASK_CONNECTNETDRV");
            LOG(L"%s", L"    --log-usage");
            LOG(L"%s", L"        sets SEE_MASK_FLAG_LOG_USAGE");
            LOG(L"%s", L"    --no-async");
            LOG(L"%s", L"        sets SEE_MASK_NOASYNC");
            LOG(L"%s", L"    --no-close-process [--relay-exit-code]");
            LOG(L"%s", L"        sets SEE_MASK_NOCLOSEPROCESS");
            LOG(L"%s", L"        optionally waits and returns the exit code");
            LOG(L"%s", L"    --no-console");
            LOG(L"%s", L"        sets SEE_MASK_NO_CONSOLE");
            LOG(L"%s", L"    --no-query-class-store");
            LOG(L"%s", L"        sets SEE_MASK_NOQUERYCLASSSTORE");
            LOG(L"%s", L"    --no-ui");
            LOG(L"%s", L"        sets SEE_MASK_FLAG_NO_UI");
            LOG(L"%s", L"    --no-zone-checks");
            LOG(L"%s", L"        sets SEE_MASK_NOZONECHECKS");
            LOG(L"%s", L"    --unicode");
            LOG(L"%s", L"        sets SEE_MASK_UNICODE");
            LOG(L"%s", L"    --wait-for-input-idle");
            LOG(L"%s", L"        sets SEE_MASK_WAITFORINPUTIDLE");
            return S_OK;
        } else if (0 == _wcsicmp(topic, L"known-folders")) {
            IKnownFolderManager *manager = nullptr;
            HRESULT hr = KnownFolders::GetManager(m_api, &manager);
            if (FAILED(hr)) {
                // GetManager logs failure
                return hr;
            }
            ReleaseOnExit releaseManager(manager);

            hr = KnownFolders::PrintKnownFolders(m_api, manager);
            if (FAILED(hr)) {
                // PrintKnownFolders logs failure
                return hr;
            }

            return S_OK;
        } else if (0 == _wcsicmp(topic, L"shellexecuteex")) {
            LOG(L"%s", L"shellexecuteex.exe");
            LOG(L"%s", L"    [");
            LOG(L"%s", L"        --file <file> |");
            LOG(L"%s", L"        --item-id-list-from-display-name <display-name> |");
            LOG(L"%s", L"        --item-id-list-from-known-folder <known-folder>");
            LOG(L"%s", L"    ]");
            LOG(L"%s", L"    [--class-name <class-name>]");
            LOG(L"%s", L"    [--directory <directory>]");
            LOG(L"%s", L"    [--parameters <parameters>]");
            LOG(L"%s", L"    [--show <show-options>]");
            LOG(L"%s", L"    [--verb <verb>]");
            LOG(L"%s", L"    <flags>");
            return S_OK;
        } else if (0 == _wcsicmp(topic, L"show-options")) {
            LOG(L"%s", L"<show-options>");
            for (int i = 0; i < _countof(showInts); i++) {
                LOG(L"    %s", showInts[i].name);
            }
            LOG(L"%s", L"");
            LOG(L"%s", L"SW_NORMAL is the default");
            return S_OK;
        } else {
            LOG(L"Unrecognized help topic %s", topic);
            return E_INVALIDARG;
        }
    } else {
        LOG(L"%s", L"pass a single topic to help");
        return E_INVALIDARG;
    }
}

void Prefs::ShowTopUsage() {
    LOG(L"%s", L"shellexecuteex.exe [-? | /? | --help]");
    LOG(L"%s", L"    show top-level usage");
    LOG(L"%s", L"");
    LOG(L"%s", L"shellexecuteex.exe help <topic>");
    LOG(L"%s", L"    show detailed help on a particular topic");
    LOG(L"%s", L"");
    LOG(L"%s", L"<topic>");
    LOG(L"%s", L"    flags");
    LOG(L"%s", L"    known-folders");
    LOG(L"%s", L"    shellexecuteex");
    LOG(L"%s", L"    show-options");
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

int Prefs::ShowInt_From_String(LPCWSTR s, bool &found) {
    for (int i = 0; i < _countof(showInts); i++) {
        if (0 == _wcsicmp(s, showInts[i].name)) {
            found = true;
            return showInts[i].value;
        }
    }

    found = false;
    return -1;
}

Prefs::ShowInt_Mapping::ShowInt_Mapping(LPCWSTR n, int v)
    : name(n), value(v) {
    }

#define SHOWINT_MAPPING_ENTRY(i) Prefs::ShowInt_Mapping(L ## #i, i)

Prefs::ShowInt_Mapping Prefs::showInts[] = {
    SHOWINT_MAPPING_ENTRY(SW_HIDE),

    // these are equal
    SHOWINT_MAPPING_ENTRY(SW_SHOWNORMAL),
    SHOWINT_MAPPING_ENTRY(SW_NORMAL),

    SHOWINT_MAPPING_ENTRY(SW_SHOWMINIMIZED),

    // these are equal
    SHOWINT_MAPPING_ENTRY(SW_SHOWMAXIMIZED),
    SHOWINT_MAPPING_ENTRY(SW_MAXIMIZE),

    SHOWINT_MAPPING_ENTRY(SW_SHOWNOACTIVATE),
    SHOWINT_MAPPING_ENTRY(SW_SHOW),
    SHOWINT_MAPPING_ENTRY(SW_MINIMIZE),
    SHOWINT_MAPPING_ENTRY(SW_SHOWMINNOACTIVE),
    SHOWINT_MAPPING_ENTRY(SW_SHOWNA),
    SHOWINT_MAPPING_ENTRY(SW_RESTORE),
    SHOWINT_MAPPING_ENTRY(SW_SHOWDEFAULT),
    SHOWINT_MAPPING_ENTRY(SW_FORCEMINIMIZE),
};