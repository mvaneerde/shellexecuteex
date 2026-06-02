#include "test-common.h"

TEST(Usage, KnownFolders_Functional) {
    WindowsApi api;

    ASSERT_HRESULT_SUCCEEDED(api.CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"help", L"known-folders"
    };

    KnownFolders knownFolders(&api);
    Usage usage(&knownFolders, &api);
    bool handled = false; // HandleHelp should flip this to true
    EXPECT_EQ(S_OK, usage.HandleHelp(_countof(argv), argv, handled));
    EXPECT_TRUE(handled);

    api.CoUninitialize();
}