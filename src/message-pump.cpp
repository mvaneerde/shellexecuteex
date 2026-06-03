#include "common.h"

MessagePump::MessagePump(IWindowsApi* api) : m_api(api) {}

struct EnumContext {
    IWindowsApi *api;
    DWORD processId;
    bool found;
};

BOOL CALLBACK AnyVisibleWindowCallback(HWND window, LPARAM param) {
    EnumContext* ctx = reinterpret_cast<EnumContext*>(param);
    DWORD pid = 0;
    ctx->api->GetWindowThreadProcessId(window, &pid);
    if (pid == ctx->processId && ctx->api->IsWindowVisible(window)) {
        ctx->found = true;
        return FALSE; // Found one, stop
    }
    return TRUE;
}

bool AnyVisibleWindowInProcess(IWindowsApi* api) {
    EnumContext ctx = { api, api->GetCurrentProcessId(), false };
    api->EnumWindows(AnyVisibleWindowCallback, reinterpret_cast<LPARAM>(&ctx));
    return ctx.found;
}

int MessagePump::WaitAndPumpMessages(HANDLE process) {
    LOG(L"%s", L"Starting message pump");

    const DWORD numHandles = (process == nullptr) ? 0 : 1;
    const HANDLE *waitHandles = (process == nullptr) ? nullptr : &process;

    int messages = 0;
    int waits = 0;
    bool processFinished = false;
    while (!processFinished) {
        DWORD waitResult = m_api->MsgWaitForMultipleObjectsEx(
            numHandles,
            waitHandles,
            (numHandles > 0) ? INFINITE : 500, // Wait briefly if no process handle
            QS_ALLINPUT,
            MWMO_INPUTAVAILABLE);
        waits++;

        if (numHandles > 0 && waitResult == WAIT_OBJECT_0) {
            processFinished = true;
        } else if (waitResult == (WAIT_OBJECT_0 + numHandles)) {
            MSG msg;
            while (m_api->PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                messages++;
                if (msg.message == WM_QUIT) {
                    LOG(L"Received WM_QUIT as message #%d", messages);
                    return (numHandles > 0) ? (int)ERROR_CANCELLED : 0;
                }
                m_api->TranslateMessage(&msg);
                m_api->DispatchMessageW(&msg);
            }
        } else if (numHandles == 0 && waitResult == WAIT_TIMEOUT) {
            // If we're not waiting for a process, only exit if no visible windows are left in this process
            if (!AnyVisibleWindowInProcess(m_api)) {
                processFinished = true;
            } else {
                // Visible windows still exist, keep pumping
            }
        } else {
            DWORD error = m_api->GetLastError();
            LOG(L"Wait failed: 0x%08x, last error %d", waitResult, error);
            return (error == ERROR_SUCCESS) ? (int)ERROR_INVALID_FUNCTION : (int)error;
        }
    }

    LOG(L"Ending message pump. Messages: %d; Waits: %d", messages, waits);

    if (process != nullptr) {
        DWORD exitCode = 0;
        if (!m_api->GetExitCodeProcess(process, &exitCode)) {
            DWORD error = m_api->GetLastError();
            LOG(L"GetExitCodeProcess failed: %d", error);
            return (error == ERROR_SUCCESS) ? (int)ERROR_INVALID_FUNCTION : (int)error;
        }
        LOG(L"Child process returned %d", exitCode);
        return (int)exitCode;
    }

    return 0;
}
