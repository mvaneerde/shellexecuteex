#include "common.h"

int wmain_testable(int argc, LPCWSTR argv[]) {
    WindowsApi api;
    KnownFolders knownFolders(&api);
    Usage usage(&knownFolders, &api);
    Prefs prefs(&api);

    return wmain_mockable(argc, argv, &api, &knownFolders, &usage, &prefs);
}

int wmain_mockable(
    int argc,
    LPCWSTR argv[],
    IWindowsApi *api,
    IKnownFolders *knownFolders,
    IUsage *usage,
    IPrefs *prefs
) {
    // some ShellExecuteEx features require working COM
    HRESULT hr = api->CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        LOG(L"CoInitializeEx failed: 0x%08x", hr);
        return hr;
    }
    CoUninitializeOnExit uninit(api);

    // handle usage statement
    bool handled = false;
    hr = usage->HandleUsage(argc, argv, handled);
    if (FAILED(hr)) {
        // HandleUsage logs failure
        return hr;
    }
    if (handled) {
        return 0;
    }

    // handle help requests
    hr = usage->HandleHelp(argc, argv, handled);
    if (FAILED(hr)) {
        // HandleHelp logs failure
        return hr;
    }
    if (handled) {
        return 0;
    }

    // parse arguments into a SHELLEXECUTEINFOW struct
    hr = prefs->Parse(argc, argv);
    if (FAILED(hr)) {
        // Prefs::Parse logs failure
        return hr;
    }

    // call ShellExecuteExW and log the result
    LPSHELLEXECUTEINFOW info = prefs->GetShellExecuteInfo();
    if (!api->ShellExecuteExW(info)) {
        DWORD error = api->GetLastError();
        LOG(L"ShellExecuteExW failed: last error %d, hInstApp: 0x%p", error, info->hInstApp);
        return error;
    }
    
    LOG(L"ShellExecuteExW succeeded");
    LOG(L"    hProcess: 0x%p", info->hProcess);
    LOG(L"    hInstApp: 0x%p", info->hInstApp);

    // wait for the process to exit and relay the exit code
    if (prefs->RelayExitCode()) {
        HANDLE process = info->hProcess;
        if (nullptr == process) {
            // No process handle was returned. This is common for Shell extensions 
            // and internal verbs (like 'properties'). We must pump messages to 
            // keep the modeless UI alive.
            LOG(L"%s", L"Process handle is null; entering message loop to host Shell UI.");
            
            MSG msg = {};
            // GetMessageW returns -1 on error, 0 on WM_QUIT, and non-zero otherwise.
            while (api->GetMessageW(&msg, nullptr, 0, 0) > 0) {
                api->TranslateMessage(&msg);
                api->DispatchMessageW(&msg);
            }
            return 0;
        }

        DWORD waitResult = api->WaitForSingleObject(process, INFINITE);
        if (WAIT_OBJECT_0 != waitResult) {
            DWORD error = api->GetLastError();
            LOG(L"WaitForSingleObject returned unexpected 0x%08x: last error %d", waitResult, error);
            if (error == ERROR_SUCCESS) {
                return ERROR_INTERNAL_ERROR;
            } else {
                return error;
            }
        }

        DWORD exitCode = 0;
        if (!api->GetExitCodeProcess(process, &exitCode)) {
            DWORD error = api->GetLastError();
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
