#include <windows.h>
#include <wlanapi.h>

int main()
{
    HANDLE wifi_handle = nullptr;
    DWORD negotiated_version = 0;
    DWORD result = WFDOpenHandle(WFD_API_VERSION, &negotiated_version, &wifi_handle);
    if (result == ERROR_SUCCESS) {
        printf("WiFi can support WifiDirect\n");
        WFDCloseHandle(wifi_handle);
        return 0;
    }
    printf("WiFi does not support WifiDirect\n");
    return 1;
}