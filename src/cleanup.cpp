#include "common.h"

CoTaskMemFreeOnExit::CoTaskMemFreeOnExit(IWindowsApi *a,PVOID m) : api(a), memory(m) {
}

CoTaskMemFreeOnExit::~CoTaskMemFreeOnExit() {
    api->CoTaskMemFree(memory);
}

CoUninitializeOnExit::CoUninitializeOnExit(IWindowsApi *a) : api(a) {
}

CoUninitializeOnExit::~CoUninitializeOnExit() {
    api->CoUninitialize();
}

FreeKnownFolderDefinitionFieldsOnExit::FreeKnownFolderDefinitionFieldsOnExit(IWindowsApi *a, KNOWNFOLDER_DEFINITION *d) : api(a), definition(d) {
}

FreeKnownFolderDefinitionFieldsOnExit::~FreeKnownFolderDefinitionFieldsOnExit() {
    api->FreeKnownFolderDefinitionFields(definition);
}

ReleaseOnExit::ReleaseOnExit(IUnknown *u) : unknown(u) {
}

ReleaseOnExit::~ReleaseOnExit() {
    unknown->Release();
}
