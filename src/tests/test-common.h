#include <gtest/gtest.h>
#include "common.h"

struct Args {
    int argc;
    LPCWSTR *argv;

    Args(int c, LPCWSTR v[]) : argc(c), argv(v) {}
};
