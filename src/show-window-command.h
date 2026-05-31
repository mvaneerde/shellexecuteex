// show-window-command.h

namespace ShowWindowCommand {
    HRESULT ParseShowWindowCommand(LPCWSTR s, int &command);
    void LogShowWindowCommands(LPCWSTR prefix);
}
