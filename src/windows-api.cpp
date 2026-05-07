#include "common.h"

BOOL WindowsApi::CloseHandle(HANDLE h) {
    return ::CloseHandle(h);
}

HWND WindowsApi::GetConsoleWindow() {
    return ::GetConsoleWindow();
}

BOOL WindowsApi::ShellExecuteExW(LPSHELLEXECUTEINFOW info) {
    return ::ShellExecuteExW(info);
}