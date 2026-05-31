#include "common.h"

namespace {
    struct ShowWindowCommandEntry {
        LPCWSTR name;
        int value;
    };

#define SHOWWINDOW_COMMAND_ENTRY(c) { L ## #c, c }

    ShowWindowCommandEntry showWindowCommands[] = {
        SHOWWINDOW_COMMAND_ENTRY(SW_HIDE),

        // these are equal
        SHOWWINDOW_COMMAND_ENTRY(SW_SHOWNORMAL),
        SHOWWINDOW_COMMAND_ENTRY(SW_NORMAL),

        SHOWWINDOW_COMMAND_ENTRY(SW_SHOWMINIMIZED),

        // these are equal
        SHOWWINDOW_COMMAND_ENTRY(SW_SHOWMAXIMIZED),
        SHOWWINDOW_COMMAND_ENTRY(SW_MAXIMIZE),

        SHOWWINDOW_COMMAND_ENTRY(SW_SHOWNOACTIVATE),
        SHOWWINDOW_COMMAND_ENTRY(SW_SHOW),
        SHOWWINDOW_COMMAND_ENTRY(SW_MINIMIZE),
        SHOWWINDOW_COMMAND_ENTRY(SW_SHOWMINNOACTIVE),
        SHOWWINDOW_COMMAND_ENTRY(SW_SHOWNA),
        SHOWWINDOW_COMMAND_ENTRY(SW_RESTORE),
        SHOWWINDOW_COMMAND_ENTRY(SW_SHOWDEFAULT),
        SHOWWINDOW_COMMAND_ENTRY(SW_FORCEMINIMIZE),
    };
}

HRESULT ShowWindowCommand::ParseShowWindowCommand(LPCWSTR s, int &command) {
    for (int i = 0; i < _countof(showWindowCommands); i++) {
        if (0 == _wcsicmp(s, showWindowCommands[i].name)) {
            command = showWindowCommands[i].value;
            return S_OK;
        }
    }

    LOG(L"Unrecognized show-window command %s", s);
    return E_INVALIDARG;
}

void ShowWindowCommand::LogShowWindowCommands(LPCWSTR prefix) {
    for (int i = 0; i < _countof(showWindowCommands); i++) {
        LOG(L"%s%s", prefix, showWindowCommands[i].name);
    }
}
