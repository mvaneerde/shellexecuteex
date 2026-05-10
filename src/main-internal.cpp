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
        DWORD error = GetLastError();
        p.LogResult(FALSE, error);
        return error;
    }

    p.LogResult(TRUE, ERROR_SUCCESS);
    return 0;
}
