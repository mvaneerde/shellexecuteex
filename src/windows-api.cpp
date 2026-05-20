#include "common.h"

HRESULT WindowsApi::CoInitializeEx(LPVOID reserved, DWORD coinit) {
    return ::CoInitializeEx(reserved, coinit);
}

void WindowsApi::CoTaskMemFree(LPVOID p) {
    return ::CoTaskMemFree(p);
}

BOOL WindowsApi::CloseHandle(HANDLE h) {
   return ::CloseHandle(h);
}

HRESULT WindowsApi::CLSIDFromString(PCWSTR s, LPCLSID c) {
    return ::CLSIDFromString(s, c);
}

void WindowsApi::CoUninitialize() {
    return ::CoUninitialize();
}

HWND WindowsApi::GetConsoleWindow() {
    return ::GetConsoleWindow();
}

BOOL WindowsApi::GetExitCodeProcess(HANDLE process, LPDWORD exitCode) {
    return ::GetExitCodeProcess(process, exitCode);
}

BOOL WindowsApi::ShellExecuteExW(LPSHELLEXECUTEINFOW info) {
    return ::ShellExecuteExW(info);
}

HRESULT WindowsApi::SHGetKnownFolderIDList(
    REFKNOWNFOLDERID id,
    DWORD flags,
    HANDLE token,
    PIDLIST_ABSOLUTE *idl
) {
    return ::SHGetKnownFolderIDList(id, flags, token, idl);
}

HRESULT WindowsApi::SHParseDisplayName(
    PCWSTR name,
    IBindCtx *context,
    PIDLIST_ABSOLUTE *idlist,
    SFGAOF in,
    SFGAOF *out
) {
    return ::SHParseDisplayName(name, context, idlist, in, out);
}


DWORD WindowsApi::WaitForSingleObject(HANDLE h, DWORD milliseconds) {
    return ::WaitForSingleObject(h, milliseconds);
}
