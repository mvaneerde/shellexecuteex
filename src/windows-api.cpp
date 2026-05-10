#include "common.h"

HRESULT WindowsApi::CoInitializeEx(LPVOID reserved, DWORD coinit) {
    return ::CoInitializeEx(reserved, coinit);
}

BOOL WindowsApi::CloseHandle(HANDLE h) {
   return ::CloseHandle(h);
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

DWORD WindowsApi::WaitForSingleObject(HANDLE h, DWORD milliseconds) {
    return ::WaitForSingleObject(h, milliseconds);
}
