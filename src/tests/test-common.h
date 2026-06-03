#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "common.h"
#include "mocks.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::SetArgReferee;

MATCHER_P(HasBit, bit, "argument has the given bit set") {
    return (arg & bit) == bit;
}

struct Args {
    int argc;
    LPCWSTR *argv;

    Args(int c, LPCWSTR v[]) : argc(c), argv(v) {}
};

bool operator==(const SHELLEXECUTEINFOW &a, const SHELLEXECUTEINFOW &b);
void ExpectEq_ShellExecuteInfoW(
    const SHELLEXECUTEINFOW &expected,
    const SHELLEXECUTEINFOW &actual);
