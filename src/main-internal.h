// main-internal.h

typedef BOOL (WINAPI *FNSHELLEXECUTEEXW)(LPSHELLEXECUTEINFOW);
int wmain_internal(int argc, LPCWSTR argv[], FNSHELLEXECUTEEXW api);
