#include <gtest/gtest.h>
#include "common.h"

struct Args {
    int argc;
    LPCWSTR *argv;

    Args(int c, LPCWSTR v[]) : argc(c), argv(v) {}
};

bool operator==(const SHELLEXECUTEINFOW &a, const SHELLEXECUTEINFOW &b);
void ExpectEq_ShellExecuteInfoW(
    const SHELLEXECUTEINFOW &expected,
    const SHELLEXECUTEINFOW &actual);
