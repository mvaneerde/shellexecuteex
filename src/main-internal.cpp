#include "common.h"

int wmain_testable(int argc, LPCWSTR argv[]) {
    WindowsApi api;
    KnownFolders knownFolders(&api);
    Usage usage({ &knownFolders, &api });
    Prefs prefs(&api);
    MessagePump messagePump(&api);

    MainContext context = { &api, &knownFolders, &usage, &prefs, &messagePump };
    return wmain_mockable(argc, argv, context);
}

int wmain_mockable(
    int argc,
    LPCWSTR argv[],
    MainContext context
) {
    // some ShellExecuteEx features require working COM
    HRESULT hr = context.api->CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        LOG(L"CoInitializeEx failed: 0x%08x", hr);
        return hr;
    }
    CoUninitializeOnExit uninit(context.api);

    // handle usage statement
    bool handled = false;
    hr = context.usage->HandleUsage(argc, argv, handled);
    if (FAILED(hr)) {
        // HandleUsage logs failure
        return hr;
    }
    if (handled) {
        return 0;
    }

    // handle help requests
    hr = context.usage->HandleHelp(argc, argv, handled);
    if (FAILED(hr)) {
        // HandleHelp logs failure
        return hr;
    }
    if (handled) {
        return 0;
    }

    // parse arguments into a SHELLEXECUTEINFOW struct
    hr = context.prefs->Parse(argc, argv);
    if (FAILED(hr)) {
        // Prefs::Parse logs failure
        return hr;
    }

    // call ShellExecuteExW and log the result
    if (!context.api->ShellExecuteExW(context.prefs->GetShellExecuteInfo())) {
        DWORD error = context.api->GetLastError();
        LOG(L"ShellExecuteExW failed: last error %d", error);
        return error;
    }
    
    LOG(L"ShellExecuteExW succeeded");
    LOG(L"    hProcess: 0x%p", context.prefs->GetShellExecuteInfo()->hProcess);
    LOG(L"    hInstApp: 0x%p", context.prefs->GetShellExecuteInfo()->hInstApp);

    if (context.prefs->RelayExitCode()) {
        HANDLE process = context.prefs->GetShellExecuteInfo()->hProcess;
        if (nullptr == process) {
            LOG(L"%s", L"Can't relay exit code because process handle is null");
            return ERROR_INVALID_HANDLE;
        }
        return context.messagePump->WaitAndPumpMessages(process);
    }

    return context.messagePump->WaitAndPumpMessages(nullptr);
}
