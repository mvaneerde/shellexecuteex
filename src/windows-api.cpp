#include "common.h"

BOOL WindowsApi::CloseHandle(HANDLE h) {
    return ::CloseHandle(h);
}

HWND WindowsApi::GetConsoleWindow() {
    return ::GetConsoleWindow();
}

BOOL WindowsApi::ShellExecuteEx(LPSHELLEXECUTEINFO info) {
    return ::ShellExecuteEx(info);
}