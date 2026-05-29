// log.h

#define LOG(format, ...) wprintf(format L"\n", __VA_ARGS__)

#define GUID_PRINTF_FORMAT L"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}"

#define GUID_PRINTF_ARGS(guid) \
    guid.Data1, guid.Data2, guid.Data3, \
    guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], \
    guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
