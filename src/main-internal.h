// main-internal.h

int wmain_testable(int argc, LPCWSTR argv[]);
int wmain_mockable(int argc, LPCWSTR argv[], IWindowsApi *api, IUsage *usage, IPrefs *prefs);
