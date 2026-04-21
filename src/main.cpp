#include "common.h"

int wmain(int argc, LPCWSTR argv[]) {
    return wmain_internal(argc, argv, ShellExecuteExW);
}