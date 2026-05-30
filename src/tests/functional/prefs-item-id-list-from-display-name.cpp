#include "test-common.h"

TEST(Prefs, ItemIdListFromDisplayName_Missing_Argument) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--item-id-list-from-display-name",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ItemIdListFromDisplayName_And_File) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--item-id-list-from-display-name", L"::{645FF040-5081-101B-9F08-00AA002F954E}",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ItemIdListFromDisplayName_Twice) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--item-id-list-from-display-name", L"::{645FF040-5081-101B-9F08-00AA002F954E}",
        L"--item-id-list-from-display-name", L"::{645FF040-5081-101B-9F08-00AA002F954E}",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_EQ(E_INVALIDARG, p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ItemIdListFromDisplayName_CanonicalName) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--item-id-list-from-display-name", L"::{645FF040-5081-101B-9F08-00AA002F954E}",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_EQ(S_OK, p.Parse(_countof(argv), argv, run));
    EXPECT_TRUE(run);
    EXPECT_EQ(SEE_MASK_IDLIST, p.fMask);
    EXPECT_NE(nullptr, p.lpIDList);
}

TEST(Prefs, ItemIdListFromDisplayName_DisplayName) {
    WindowsApi api;

    ASSERT_HRESULT_SUCCEEDED(api.CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--item-id-list-from-display-name", L"shell:RecycleBinFolder",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_EQ(S_OK, p.Parse(_countof(argv), argv, run));
    EXPECT_TRUE(run);
    EXPECT_EQ(SEE_MASK_IDLIST, p.fMask);
    EXPECT_NE(nullptr, p.lpIDList);
    CoUninitialize();
}

TEST(Prefs, ItemIdListFromDisplayName_Invalid) {
    WindowsApi api;

    EXPECT_HRESULT_SUCCEEDED(api.CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--item-id-list-from-display-name", L"foo",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_EQ(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), p.Parse(_countof(argv), argv, run));
    // no expectation on run

    api.CoUninitialize();
}
