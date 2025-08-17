// https://learn.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlanenuminterfaces
// https://learn.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlangetavailablenetworklist

#include <windows.h>
#include <wlanapi.h>
#include <iostream>

// #include <wtypes.h>
#include <stdio.h> //For wprintf

int main()
{
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    int iRet = 0;
    WCHAR GuidString[40] = {0};

    PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;
    PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;

    int iRSSI = 0;

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS)
    {
        std::cout << "WlanOpenHandle failed with error: " << dwResult << std::endl;
        return 1;
    }
    std::cout << "WlanOpenHandle success" << std::endl;

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS)
    {
        std::cout << "WlanOpenHandle failed with error: " << dwResult << std::endl;
        return 1;
    }

    std::cout << "Num Entries: " << pIfList->dwNumberOfItems << std::endl;
    std::cout << "Current Index: " << pIfList->dwIndex << std::endl;

    for (int i = 0; i < (int)pIfList->dwNumberOfItems; i++)
    {
        pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
        std::cout << "Interface Index: " << i << std::endl;

        std::cout << "Interface Description: " << pIfInfo->strInterfaceDescription << std::endl;
        std::wcout << "Interface Description: " << pIfInfo->strInterfaceDescription << std::endl;
        wprintf(L"Interface Description[%d]: %S\n", i, pIfInfo->strInterfaceDescription);

        iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString, 39);
        // For c rather than C++ source code, the above line needs to be
        // iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 39);
        if (iRet == 0)
            wprintf(L"StringFromGUID2 failed\n");
        else
        {
            wprintf(L"InterfaceGUID[%d]: %S\n", i, GuidString);
        }

        std::cout << "Interface State: " << pIfInfo->isState << std::endl;
        switch (pIfInfo->isState)
        {
        case wlan_interface_state_not_ready:
            std::cout << "Not ready" << std::endl;
            break;
        case wlan_interface_state_connected:
            std::cout << "Connected" << std::endl;
            break;
        case wlan_interface_state_ad_hoc_network_formed:
            std::cout << "First node in a ad hoc network" << std::endl;
            break;
        case wlan_interface_state_disconnecting:
            std::cout << "Disconnecting" << std::endl;
            break;
        case wlan_interface_state_disconnected:
            std::cout << "Not connected" << std::endl;
            break;
        case wlan_interface_state_associating:
            std::cout << "Attempting to associate with a network" << std::endl;
            break;
        case wlan_interface_state_discovering:
            std::cout << "Auto configuration is discovering settings for the network" << std::endl;
            break;
        case wlan_interface_state_authenticating:
            std::cout << "In process of authenticating" << std::endl;
            break;
        default:
            std::cout << "Unknown state " << pIfInfo->isState << std::endl;
            break;
        }
        std::cout << std::endl;
    }

    dwResult = WlanGetAvailableNetworkList(hClient, &pIfInfo->InterfaceGuid, 0, NULL, &pBssList);
    if (dwResult != ERROR_SUCCESS)
    {
        wprintf(L"WlanGetAvailableNetworkList failed with error: %u\n", dwResult);
        return 1;
    }
    wprintf(L"WLAN_AVAILABLE_NETWORK_LIST for this interface\n");
    wprintf(L"  Num Entries: %lu\n\n", pBssList->dwNumberOfItems);

    for (int j = 0; j < pBssList->dwNumberOfItems; j++)
    {
        pBssEntry = (WLAN_AVAILABLE_NETWORK *)&pBssList->Network[j];

        wprintf(L"  Profile Name[%u]:  %S\n", j, pBssEntry->strProfileName);

        wprintf(L"  SSID[%u]:\t\t ", j);
        if (pBssEntry->dot11Ssid.uSSIDLength == 0)
            wprintf(L"\n");
        else
        {
            for (int k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++)
            {
                wprintf(L"%c", (int)pBssEntry->dot11Ssid.ucSSID[k]);
            }
            wprintf(L"\n");
        }

        wprintf(L"  BSS Network type[%u]:\t ", j);
        switch (pBssEntry->dot11BssType)
        {
        case dot11_BSS_type_infrastructure:
            wprintf(L"Infrastructure (%u)\n", pBssEntry->dot11BssType);
            break;
        case dot11_BSS_type_independent:
            wprintf(L"Infrastructure (%u)\n", pBssEntry->dot11BssType);
            break;
        default:
            wprintf(L"Other (%lu)\n", pBssEntry->dot11BssType);
            break;
        }

        wprintf(L"  Number of BSSIDs[%u]:\t %u\n", j, pBssEntry->uNumberOfBssids);

        wprintf(L"  Connectable[%u]:\t ", j);
        if (pBssEntry->bNetworkConnectable)
            wprintf(L"Yes\n");
        else
        {
            wprintf(L"No\n");
            wprintf(L"  Not connectable WLAN_REASON_CODE value[%u]:\t %u\n", j,
                    pBssEntry->wlanNotConnectableReason);
        }

        wprintf(L"  Number of PHY types supported[%u]:\t %u\n", j, pBssEntry->uNumberOfPhyTypes);

        if (pBssEntry->wlanSignalQuality == 0)
            iRSSI = -100;
        else if (pBssEntry->wlanSignalQuality == 100)
            iRSSI = -50;
        else
            iRSSI = -100 + (pBssEntry->wlanSignalQuality / 2);

        wprintf(L"  Signal Quality[%u]:\t %u (RSSI: %i dBm)\n", j,
                pBssEntry->wlanSignalQuality, iRSSI);

        wprintf(L"  Security Enabled[%u]:\t ", j);
        if (pBssEntry->bSecurityEnabled)
            wprintf(L"Yes\n");
        else
            wprintf(L"No\n");

        wprintf(L"  Default AuthAlgorithm[%u]: ", j);
        switch (pBssEntry->dot11DefaultAuthAlgorithm)
        {
        case DOT11_AUTH_ALGO_80211_OPEN:
            wprintf(L"802.11 Open (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            break;
        case DOT11_AUTH_ALGO_80211_SHARED_KEY:
            wprintf(L"802.11 Shared (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            break;
        case DOT11_AUTH_ALGO_WPA:
            wprintf(L"WPA (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            break;
        case DOT11_AUTH_ALGO_WPA_PSK:
            wprintf(L"WPA-PSK (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            break;
        case DOT11_AUTH_ALGO_WPA_NONE:
            wprintf(L"WPA-None (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            break;
        case DOT11_AUTH_ALGO_RSNA:
            wprintf(L"RSNA (%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            break;
        case DOT11_AUTH_ALGO_RSNA_PSK:
            wprintf(L"RSNA with PSK(%u)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            break;
        default:
            wprintf(L"Other (%lu)\n", pBssEntry->dot11DefaultAuthAlgorithm);
            break;
        }

        wprintf(L"  Default CipherAlgorithm[%u]: ", j);
        switch (pBssEntry->dot11DefaultCipherAlgorithm)
        {
        case DOT11_CIPHER_ALGO_NONE:
            wprintf(L"None (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            break;
        case DOT11_CIPHER_ALGO_WEP40:
            wprintf(L"WEP-40 (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            break;
        case DOT11_CIPHER_ALGO_TKIP:
            wprintf(L"TKIP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            break;
        case DOT11_CIPHER_ALGO_CCMP:
            wprintf(L"CCMP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            break;
        case DOT11_CIPHER_ALGO_WEP104:
            wprintf(L"WEP-104 (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            break;
        case DOT11_CIPHER_ALGO_WEP:
            wprintf(L"WEP (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            break;
        default:
            wprintf(L"Other (0x%x)\n", pBssEntry->dot11DefaultCipherAlgorithm);
            break;
        }

        wprintf(L"  Flags[%u]:\t 0x%x", j, pBssEntry->dwFlags);
        if (pBssEntry->dwFlags)
        {
// available network flags
#define WLAN_AVAILABLE_NETWORK_CONNECTED 0x00000001              // This network is currently connected
#define WLAN_AVAILABLE_NETWORK_HAS_PROFILE 0x00000002            // There is a profile for this network
#define WLAN_AVAILABLE_NETWORK_CONSOLE_USER_PROFILE 0x00000004   // The profile is the active console user's per user profile
#define WLAN_AVAILABLE_NETWORK_INTERWORKING_SUPPORTED 0x00000008 // Interworking is supported
#define WLAN_AVAILABLE_NETWORK_HOTSPOT2_ENABLED 0x00000010       // Hotspot2 is enabled
#define WLAN_AVAILABLE_NETWORK_ANQP_SUPPORTED 0x00000020         // ANQP is supported
#define WLAN_AVAILABLE_NETWORK_HOTSPOT2_DOMAIN 0x00000040        // Domain network
#define WLAN_AVAILABLE_NETWORK_HOTSPOT2_ROAMING 0x00000080       // Roaming network
#define WLAN_AVAILABLE_NETWORK_AUTO_CONNECT_FAILED 0x00000100    // This network failed to connect
            if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
                wprintf(L" - Currently connected");
            if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE)
                wprintf(L" - Has profile");
        }
        wprintf(L"\n");

        wprintf(L"\n");
    }

    if (pBssList != NULL)
    {
        WlanFreeMemory(pBssList);
        pBssList = NULL;
    }

    if (pIfList != NULL)
    {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }
    return 0;
}
