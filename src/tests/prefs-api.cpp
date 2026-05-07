#include "test-common.h"

TEST(Prefs, Apis) {
    MockWindowsApi mockApi;
    HANDLE fakeHandle = reinterpret_cast<HANDLE>(0x12345678);

    // The Prefs constructor calls GetConsoleWindow; we set a default expectation.
    EXPECT_CALL(mockApi, GetConsoleWindow()).WillOnce(Return((HWND)nullptr));

    // We expect CloseHandle to be called exactly once with our specific handle.
    EXPECT_CALL(mockApi, CloseHandle(fakeHandle)).Times(1).WillOnce(Return(TRUE));

    {
        Prefs prefs(&mockApi);
        prefs.hProcess = fakeHandle; // Simulate a handle acquired from a successful execution
    } // Destructor is invoked here, triggering the CloseHandle call
}
