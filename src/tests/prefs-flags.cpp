#include "test-common.h"

// simple flags
TEST(Prefs, Flags) {
    std::pair<LPCWSTR, DWORD> flags[] = {
        {
            L"--no-close-process",
            SEE_MASK_NOCLOSEPROCESS
        },
        {
            L"--connect-network-drive",
            SEE_MASK_CONNECTNETDRV
        },
        {
            L"--no-async",
            SEE_MASK_NOASYNC
        },
        {
            L"--do-environment-substitution",
            SEE_MASK_DOENVSUBST
        },
        {
            L"--no-ui",
            SEE_MASK_FLAG_NO_UI
        },
        {
            L"--unicode",
            SEE_MASK_UNICODE
        },
        {
            L"--no-console",
            SEE_MASK_NO_CONSOLE
        },
        {
            L"--async-ok",
            SEE_MASK_ASYNCOK
        },
        {
            L"--no-query-class-store",
            SEE_MASK_NOQUERYCLASSSTORE
        },
        {
            L"--no-zone-checks",
            SEE_MASK_NOZONECHECKS
        },
        {
            L"--wait-for-input-idle",
            SEE_MASK_WAITFORINPUTIDLE
        },
        {
            L"--log-usage",
            SEE_MASK_FLAG_LOG_USAGE
        },
    };
    
    for (auto f : flags) {
        // valid case - passing the argument sets the flag
        {            
            LOG(L"Checking that %s sets 0x%08x", f.first, f.second);

            LPCWSTR argv[] = {
                L"ShellExecuteEx.exe",
                L"--file", L"notepad.exe",
                L"--show", L"SW_NORMAL",
                f.first
            };

            Prefs p;
            bool run = false;
            EXPECT_EQ(p.Parse(_countof(argv), argv, run), true);
            EXPECT_EQ(run, true);

            SHELLEXECUTEINFOW expected = { sizeof(expected) };
            expected.lpFile = L"notepad.exe";
            expected.nShow = SW_NORMAL;
            expected.fMask = f.second;

            ExpectEq_ShellExecuteInfoW(expected, p);
        }

        // invalid case - passing argument twice
        {
            LOG(L"Checking that %s twice fails", f.first);

            LPCWSTR argv[] = {
                L"ShellExecuteEx.exe",
                L"--file", L"notepad.exe",
                L"--show", L"SW_NORMAL",
                f.first,
                f.first
            };

            Prefs p;
            bool run = false;
            EXPECT_EQ(p.Parse(_countof(argv), argv, run), false);
            // no expectation on run
        }
    }
}
