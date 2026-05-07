#include "common.h"

int wmain(int argc, LPCWSTR argv[]) {
    WindowsApi api;
    return wmain_internal(argc, argv, &api);
}