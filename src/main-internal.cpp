#include "common.h"

int wmain_internal(int argc, LPCWSTR argv[], IWindowsApi *api) {
    // some ShellExecuteEx features require working COM
    HRESULT hr = api->CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        LOG(L"CoInitializeEx failed: 0x%08x", hr);
        return hr;
    }
    CoUninitializeOnExit uninit(api);

    // parse command-line arguments
    bool run = false;
    Prefs p(api); // inherits from SHELLEXECUTEINFOW
    if (!p.Parse(argc, argv, run)) {
        // parsing failed
        // the particular failure is logged inside Prefs::Parse
        return ERROR_INVALID_PARAMETER;
    }

    if (!run) {
        // just a usage statement
        return 0;
    }

    // actually invoke the API and log the result
    if (!api->ShellExecuteExW(&p)) {
        DWORD error = api->GetLastError();
        return p.LogResult(FALSE, error);
    }

    p.LogResult(TRUE, ERROR_SUCCESS);

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
