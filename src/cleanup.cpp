#include "common.h"

CoTaskMemFreeOnExit::CoTaskMemFreeOnExit(PVOID m) : memory(m) {
}

CoTaskMemFreeOnExit::~CoTaskMemFreeOnExit() {
    CoTaskMemFree(memory);
}

CoUninitializeOnExit::CoUninitializeOnExit(IWindowsApi *a) : api(a) {
}

CoUninitializeOnExit::~CoUninitializeOnExit() {
    api->CoUninitialize();
}

FreeKnownFolderDefinitionFieldsOnExit::FreeKnownFolderDefinitionFieldsOnExit(KNOWNFOLDER_DEFINITION *d) : definition(d) {
}

FreeKnownFolderDefinitionFieldsOnExit::~FreeKnownFolderDefinitionFieldsOnExit() {
    FreeKnownFolderDefinitionFields(definition);
}

ReleaseOnExit::ReleaseOnExit(IUnknown *u) : unknown(u) {
}

ReleaseOnExit::~ReleaseOnExit() {
    unknown->Release();
}
