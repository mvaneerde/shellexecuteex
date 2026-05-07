#include "common.h"

int wmain_internal(int argc, LPCWSTR argv[], IWindowsApi *api) {
    bool run = false;

    Prefs p(api); // inherits from SHELLEXECUTEINFOW
    if (p.Parse(argc, argv, run)) {
        if (run) {
            // actually invoke the API and log the result
            BOOL result = api->ShellExecuteExW(&p);
            DWORD error = (result ? ERROR_SUCCESS : GetLastError());
            p.LogResult(result, error);
            return (result ? 0 : error);
        } else {
            // just a usage statement
            return 0;
        }
    } else {
        // parsing failed
        return ERROR_INVALID_PARAMETER;
    }

    // unreachable
}
