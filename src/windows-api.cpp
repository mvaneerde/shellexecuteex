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

BOOL WindowsApi::EnumWindows(WNDENUMPROC callback, LPARAM param) {
    return ::EnumWindows(callback, param);
}

BOOL WindowsApi::EnumThreadWindows(DWORD threadId, WNDENUMPROC callback, LPARAM param) {
    return ::EnumThreadWindows(threadId, callback, param);
}

void WindowsApi::FreeKnownFolderDefinitionFields(KNOWNFOLDER_DEFINITION *d) {
    return ::FreeKnownFolderDefinitionFields(d);
}

HWND WindowsApi::GetConsoleWindow() {
    return ::GetConsoleWindow();
}

DWORD WindowsApi::GetCurrentProcessId() {
    return ::GetCurrentProcessId();
}

DWORD WindowsApi::GetCurrentThreadId() {
    return ::GetCurrentThreadId();
}

DWORD WindowsApi::GetWindowThreadProcessId(HWND window, LPDWORD processId) {
    return ::GetWindowThreadProcessId(window, processId);
}

BOOL WindowsApi::GetExitCodeProcess(HANDLE process, LPDWORD exitCode) {
    return ::GetExitCodeProcess(process, exitCode);
}

DWORD WindowsApi::GetLastError() {
    return ::GetLastError();
}

HRESULT WindowsApi::IIDFromString(PCWSTR s, LPIID iid) {
    return ::IIDFromString(s, iid);
}

DWORD WindowsApi::MsgWaitForMultipleObjectsEx(DWORD count, const HANDLE *handles, DWORD milliseconds, DWORD wakeMask, DWORD flags) {
    return ::MsgWaitForMultipleObjectsEx(count, handles, milliseconds, wakeMask, flags);
}

BOOL WindowsApi::PeekMessageW(LPMSG msg, HWND window, UINT filterMin, UINT filterMax, UINT removeMsg) {
    return ::PeekMessageW(msg, window, filterMin, filterMax, removeMsg);
}

BOOL WindowsApi::ShellExecuteExW(LPSHELLEXECUTEINFOW info) {
    return ::ShellExecuteExW(info);
}

BOOL WindowsApi::IsWindowVisible(HWND window) {
    return ::IsWindowVisible(window);
}

BOOL WindowsApi::TranslateMessage(const MSG *msg) {
    return ::TranslateMessage(msg);
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
