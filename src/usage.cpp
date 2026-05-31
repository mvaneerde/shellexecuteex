#include "common.h"

Usage::Usage(IWindowsApi *api) : m_api(api) {}

HRESULT Usage::HandleUsage(int argc, LPCWSTR argv[], bool &handled) {
    if (IsUsage(argc, argv)) {
        ShowUsage();
        handled = true;
    }

    return S_OK;
}
    
HRESULT Usage::HandleHelp(int argc, LPCWSTR argv[], bool &handled) {
    bool help = false;
    LPCWSTR topic = nullptr;
    HRESULT hr = IsHelp(argc, argv, help, topic);
    if (FAILED(hr)) {
        // IsHelp logs failure
        return hr;
    }

    if (help) {
        hr = ShowHelp(topic);

        if (FAILED(hr)) {
            // ShowHelp logs failure 
            return hr;
        }

        handled = true;
    }

    return S_OK;
}

bool Usage::IsUsage(int argc, LPCWSTR argv[])
{
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

void Usage::ShowUsage() {
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

HRESULT Usage::IsHelp(int argc, LPCWSTR argv[], bool &help, LPCWSTR &topic) {
    help = (argc >= 1 + 1 && 0 == _wcsicmp(argv[1], L"help"));

    if (help) {
        if (argc != 2 + 1) {
            LOG(L"%s", L"help requires a single topic");
            return E_INVALIDARG;
        }

        topic = argv[2];
    }
    
    return S_OK;
}

HRESULT Usage::ShowHelp(LPCWSTR topic) {
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
        return ShowHelpKnownFolders();
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
        ShowWindowCommand::LogShowWindowCommands(L"    ");
        return S_OK;
    } else {
        LOG(L"Unrecognized help topic %s", topic);
        return E_INVALIDARG;
    }
}

HRESULT Usage::ShowHelpKnownFolders() {
    LOG(L"%s", L"These are the known folders on your system:");

    KnownFolders knownFolders(m_api);
    IKnownFolderManager *manager = nullptr;
    HRESULT hr = knownFolders.GetManager(&manager);
    if (FAILED(hr)) {
        // GetManager logs failure
        return hr;
    }
    ReleaseOnExit releaseManager(manager);

    hr = knownFolders.PrintKnownFolders(manager);
    if (FAILED(hr)) {
        // PrintKnownFolders logs failure
        return hr;
    }

    return S_OK;
}