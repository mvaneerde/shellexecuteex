#include "test-common.h"

int wmain(int argc, LPCWSTR argv[]) {
    if (SUCCEEDED(::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {
        wchar_t **v = const_cast<wchar_t **>(argv);
        ::testing::InitGoogleTest(&argc, v);
        int ret = RUN_ALL_TESTS();
        ::CoUninitialize();
        return ret;
    } else {
        return 1;
    }
}
