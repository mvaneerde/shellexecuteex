#include "common.h"

HRESULT WindowsApi::CoInitializeEx(LPVOID reserved, DWORD coinit) {
    return ::CoInitializeEx(reserved, coinit);
}

BOOL WindowsApi::CloseHandle(HANDLE h) {
   return ::CloseHandle(h);
}

HRESULT WindowsApi::CoCreateInstance(
    REFCLSID clsid,
    LPUNKNOWN outer,
    DWORD context,
    REFIID iid,
    LPVOID *out
) {
    return ::CoCreateInstance(clsid, outer, context, iid, out);
}

void WindowsApi::CoTaskMemFree(LPVOID p) {
    return ::CoTaskMemFree(p);
}

void WindowsApi::CoUninitialize() {
    return ::CoUninitialize();
}

LRESULT WindowsApi::DispatchMessageW(const MSG *msg) {
    return ::DispatchMessageW(msg);
}

void WindowsApi::FreeKnownFolderDefinitionFields(KNOWNFOLDER_DEFINITION *d) {
    return ::FreeKnownFolderDefinitionFields(d);
}

HWND WindowsApi::GetConsoleWindow() {
    return ::GetConsoleWindow();
}

BOOL WindowsApi::GetExitCodeProcess(HANDLE process, LPDWORD exitCode) {
    return ::GetExitCodeProcess(process, exitCode);
}

BOOL WindowsApi::GetMessageW(LPMSG msg, HWND wnd, UINT filterMin, UINT filterMax) {
    return ::GetMessageW(msg, wnd, filterMin, filterMax);
}

DWORD WindowsApi::GetLastError() {
    return ::GetLastError();
}

HRESULT WindowsApi::IIDFromString(PCWSTR s, LPIID iid) {
    return ::IIDFromString(s, iid);
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

BOOL WindowsApi::TranslateMessage(const MSG *lpMsg) {
    return ::TranslateMessage(lpMsg);
}

DWORD WindowsApi::WaitForSingleObject(HANDLE h, DWORD milliseconds) {
    return ::WaitForSingleObject(h, milliseconds);
}
