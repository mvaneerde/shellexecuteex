#include "common.h"

CoUninitializeOnExit::CoUninitializeOnExit(IWindowsApi *a) : api(a) {
}

CoUninitializeOnExit::~CoUninitializeOnExit() {
    api->CoUninitialize();
}
