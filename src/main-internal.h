// main-internal.h

typedef BOOL (*FNSHELLEXECUTEEXW)(LPSHELLEXECUTEINFOW);
int wmain_internal(int argc, LPCWSTR argv[], FNSHELLEXECUTEEXW api);
