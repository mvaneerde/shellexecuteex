// cleanup.h

class CoUninitializeOnExit {
public:
    CoUninitializeOnExit(IWindowsApi *a);
    ~CoUninitializeOnExit();

private:
    IWindowsApi *api;
};
