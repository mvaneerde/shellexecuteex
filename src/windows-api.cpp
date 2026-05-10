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

BOOL WindowsApi::ShellExecuteExW(LPSHELLEXECUTEINFOW info) {
    return ::ShellExecuteExW(info);
}