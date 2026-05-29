#include "test-common.h"

TEST(Prefs, ItemIdListFromKnownFolderId_Missing_Argument) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--item-id-list-from-known-folder-id",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_FALSE(p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ItemIdListFromKnownFolderId_And_File) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--file", L"notepad.exe",
        L"--item-id-list-from-known-folder-id", L"{645FF040-5081-101B-9F08-00AA002F954E}",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_FALSE(p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ItemIdListFromKnownFolderId_Twice) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--item-id-list-from-known-folder-id", L"{B7534046-3ECB-4C18-BE4E-64CD4CB7D6AC}",
        L"--item-id-list-from-known-folder-id", L"{B7534046-3ECB-4C18-BE4E-64CD4CB7D6AC}",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_FALSE(p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ItemIdListFromKnownFolderId_NotGuid) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--item-id-list-from-known-folder-id", L"foo",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_FALSE(p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ItemIdListFromKnownFolderId_NullGuid) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--item-id-list-from-known-folder-id", L"{00000000-0000-0000-0000-000000000000}",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_FALSE(p.Parse(_countof(argv), argv, run));
    // no expectation on run
}

TEST(Prefs, ItemIdListFromKnownFolderId_Valid) {
    WindowsApi api;

    LPCWSTR argv[] = {
        L"shellexecuteex.exe",
        L"--item-id-list-from-known-folder-id", L"{B7534046-3ECB-4C18-BE4E-64CD4CB7D6AC}",
    };

    Prefs p(&api);
    bool run = false;
    EXPECT_TRUE(p.Parse(_countof(argv), argv, run));
    EXPECT_TRUE(run);
    EXPECT_EQ(SEE_MASK_IDLIST, p.fMask);
    EXPECT_NE(nullptr, p.lpIDList);
}
