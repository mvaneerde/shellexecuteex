#include "common.h"

Prefs::Prefs(IWindowsApi *api) :
    SHELLEXECUTEINFOW { sizeof(SHELLEXECUTEINFOW) },
    m_api(api)
{
    hwnd = m_api->GetConsoleWindow();
    nShow = -1; // required parameter
}

Prefs::~Prefs() {
    if (hProcess != nullptr) {
        m_api->CloseHandle(hProcess);
    }
}

bool Prefs::Parse(int argc, LPCWSTR argv[], bool &run) {
    // see if this is a request for a usage statement
    if (IsUsage(argc, argv)) {
        ShowUsage();
        run = false;
        return true;
    } else {
        // some arguments just set a flag
        class FlagArg {
        public:
            ULONG flag;
            bool seen;

            FlagArg() : flag(0), seen(false) {}
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

            StringArg() : setting(nullptr), seen(false) {}
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
        bool seenShow = false;

        for (int i = 1; i < argc; i++) {
            // flag arguments
            {
                auto it = flagArgs.find(argv[i]);
                if (it != flagArgs.end()) {
                    FlagArg &arg = it->second;
                    if (arg.seen) {
                        LOG(L"Multiple %s arguments passed", it->first);
                        return false;
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
                        return false;
                    }
                    arg.seen = true;

                    if (++i == argc) {
                        LOG(L"%s requires a value", it->first);
                        return false;
                    }

                    *arg.setting = argv[i];
                    continue;
                }
            }

            // --class-name
            if (0 == _wcsicmp(argv[i], L"--class-name")) {
                if (seenClass) {
                    LOG(L"%s", L"Multiple class arguments passed");
                    return false;
                }
                seenClass = true;

                i++;
                if (i == argc) {
                    LOG(L"%s", L"--class-name requires a value");
                    return false;
                }

                fMask |= SEE_MASK_CLASSNAME;
                lpClass = argv[i];
                continue;
            }

            // --show
            if (0 == _wcsicmp(argv[i], L"--show")) {
                if (seenShow) {
                    LOG(L"%s", L"Multiple --show arguments passed");
                    return false;
                }
                seenShow = true;

                i++;
                if (i == argc) {
                    LOG(L"%s", L"--show requires a value");
                    return false;
                }

                bool found = false;
                nShow = ShowInt_From_String(argv[i], found);
                if (!found) {
                    LOG(L"%s", L"Unrecognized value for --show");
                    return false;
                }
                continue;
            }

            // any other argument
            LOG(L"Unrecognized argument %s", argv[i]);
            return false;
        }

        // file is required
        if (!stringArgs[L"--file"].seen) {
            LOG(L"%s", L"--file is required");
            return false;
        }

        // show is required
        if (!seenShow) {
            LOG(L"%s", L"--show is required");
            return false;
        }

        run = true;
        return true;
    }

    // unreachable
}

bool Prefs::IsUsage(int argc, LPCWSTR argv[]) {
    // no parameters
    if (argc == 1) {
        return true;
    }

    // one parameter, -? or equivalent
    if (argc == 2) {
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

void Prefs::ShowUsage() {
    LOG(L"%s", L"shellexecute.exe [-? | /? | --help]");
    LOG(L"%s", L"    show usage");
    LOG(L"%s", L"");
    LOG(L"%s", L"shellexecute.exe");
    LOG(L"%s", L"    [--verb <verb>]");
    LOG(L"%s", L"    --file <file>");
    LOG(L"%s", L"    [--parameters <parameters>]");
    LOG(L"%s", L"    [--directory <directory>]");
    LOG(L"%s", L"    --show <show-options>");
    LOG(L"%s", L"    [--class-name <class-name>]");
    LOG(L"%s", L"    [--no-close-process]");
    LOG(L"%s", L"    [--connect-network-drive]");
    LOG(L"%s", L"    [--no-async]");
    LOG(L"%s", L"    [--do-environment-substitution]");
    LOG(L"%s", L"    [--no-ui]");
    LOG(L"%s", L"    [--unicode]");
    LOG(L"%s", L"    [--no-console]");
    LOG(L"%s", L"    [--async-ok]");
    LOG(L"%s", L"    [--no-query-class-store]");
    LOG(L"%s", L"    [--no-zone-checks]");
    LOG(L"%s", L"    [--wait-for-input-idle]");
    LOG(L"%s", L"    [--log-usage]");

    LOG(L"%s", L"");
    LOG(L"%s", L"show-options:");
    for (int i = 0; i < _countof(showInts); i++) {
        LOG(L"    %s%s",
            showInts[i].name,
            (i == _countof(showInts) - 1 ? L"" : L" |"));
    }
}

void Prefs::LogResult(BOOL result, DWORD error) {
    LOG(L"ShellExecuteExW %s", (result ? L"succeeded" : L"failed"));
    if (result) {
        LOG(L"hProcess: 0x%p", hProcess);
    } else {
        LOG(L"Last error: %d", error);
    }
    LOG(L"hInstApp: 0x%p", hInstApp);
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

Prefs::ShowInt_Mapping Prefs::showInts[14] = {
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