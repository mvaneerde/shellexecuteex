#include "test-common.h"

TEST(Help, KnownFolders_Functional) {
    WindowsApi api;

    ASSERT_HRESULT_SUCCEEDED(api.CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"help", L"known-folders"
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_HRESULT_SUCCEEDED(p.Parse(_countof(argv), argv, run));
    EXPECT_FALSE(run);
    api.CoUninitialize();
}