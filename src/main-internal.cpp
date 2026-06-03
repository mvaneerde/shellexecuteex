#include "common.h"

int wmain_testable(int argc, LPCWSTR argv[]) {
    WindowsApi api;
    KnownFolders knownFolders(&api);
    Usage usage({ &knownFolders, &api });
    Prefs prefs(&api);

    MainContext context = { &api, &knownFolders, &usage, &prefs };
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

    // wait for the process to exit and relay the exit code
    if (context.prefs->RelayExitCode()) {
        HANDLE process = context.prefs->GetShellExecuteInfo()->hProcess;
        if (nullptr == process) {
            LOG(L"%s", L"Can't relay exit code because process handle is null");
            return ERROR_INVALID_HANDLE;
        }

        DWORD waitResult = context.api->WaitForSingleObject(process, INFINITE);
        if (WAIT_OBJECT_0 != waitResult) {
            DWORD error = context.api->GetLastError();
            LOG(L"WaitForSingleObject returned unexpected 0x%08x: last error %d", waitResult, error);
            if (error == ERROR_SUCCESS) {
                return ERROR_INTERNAL_ERROR;
            } else {
                return error;
            }
        }

        DWORD exitCode = 0;
        if (!context.api->GetExitCodeProcess(process, &exitCode)) {
            DWORD error = context.api->GetLastError();
            LOG(L"GetExitCodeProcess failed: last error %d", error);
            if (error == ERROR_SUCCESS) {
                return ERROR_INTERNAL_ERROR;
            } else {
                return error;
            }
        }

        LOG(L"Child process returned %d", exitCode);
        return exitCode;
    }

    return 0;
}
