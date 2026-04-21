// main-internal.h

typedef BOOL (*FNSHELLEXECUTEXW)(LPSHELLEXECUTEINFOW);
int wmain_internal(int argc, LPCWSTR argv[], FNSHELLEXECUTEXW api);
