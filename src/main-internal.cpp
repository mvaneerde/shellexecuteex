#include "common.h"

int wmain_internal(int argc, LPCWSTR argv[], IWindowsApi *api) {
    // some ShellExecuteEx features require working COM
    HRESULT hr = api->CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        LOG(L"CoInitializeEx failed: 0x%08x", hr);
        return hr;
    }
    CoUninitializeOnExit uninit(api);

    // handle usage statement
    Usage usage(api);
    bool handled = false;
    hr = usage.HandleUsage(argc, argv, handled);
    if (FAILED(hr)) {
        // HandleUsage logs failure
        return hr;
    }
    if (handled) {
        return 0;
    }

    // handle help requests
    hr = usage.HandleHelp(argc, argv, handled);
    if (FAILED(hr)) {
        // HandleHelp logs failure
        return hr;
    }
    if (handled) {
        return 0;
    }

    // parse arguments into a SHELLEXECUTEINFOW struct
    Prefs p(api); // inherits from SHELLEXECUTEINFOW
    hr = p.Parse(argc, argv);
    if (FAILED(hr)) {
        // Prefs::Parse logs failure
        return hr;
    }

    // call ShellExecuteExW and log the result
    if (!api->ShellExecuteExW(&p)) {
        DWORD error = api->GetLastError();
        return p.LogResult(FALSE, error);
    }
    p.LogResult(TRUE, ERROR_SUCCESS);

    // wait for the process to exit and relay the exit code
    if (p.RelayExitCode()) {
        if (nullptr == p.hProcess) {
            LOG(L"%s", L"Can't relay exit code because process handle is null");
            return ERROR_INVALID_HANDLE;
        }

        api->WaitForSingleObject(p.hProcess, INFINITE);
        DWORD exitCode = 0;
        if (!api->GetExitCodeProcess(p.hProcess, &exitCode)) {
            DWORD error = api->GetLastError();
            LOG(L"GetExitCodeProcess failed: 0x%08x", error);
            return error;
        }

        LOG(L"Child process returned %d", exitCode);
        return exitCode;
    }

    return 0;
}
