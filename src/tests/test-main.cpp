#include "test-common.h"

int wmain(int argc, LPCWSTR argv[]) {
    wchar_t **v = const_cast<wchar_t **>(argv);
    ::testing::InitGoogleTest(&argc, v);
    int ret = RUN_ALL_TESTS();
    return ret;
}
