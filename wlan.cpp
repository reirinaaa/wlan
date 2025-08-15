// https://learn.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlanenuminterfaces

#include <windows.h>
#include <wlanapi.h>
#include <iostream>

// #include <wtypes.h>
#include <stdio.h>

int main()
{
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient); 
    if (dwResult != ERROR_SUCCESS)  {
        std::cout << "WlanOpenHandle failed with error: " << dwResult << std::endl;
        return 1;
    }
    std::cout << "WlanOpenHandle success" << std::endl;

    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList); 
    if (dwResult != ERROR_SUCCESS)  {
        std::cout << "WlanOpenHandle failed with error: " << dwResult << std::endl;
        return 1;
    }

    std::cout << "Num Entries: " << pIfList->dwNumberOfItems << std::endl;
    std::cout << "Current Index: " << pIfList->dwIndex << std::endl;

    for (int i = 0; i < (int) pIfList->dwNumberOfItems; i++) {
        pIfInfo = (WLAN_INTERFACE_INFO *) &pIfList->InterfaceInfo[i];
        std::cout << "Interface Index: " << i << std::endl;
        std::cout << "Interface Description: " << pIfInfo->strInterfaceDescription << std::endl;
        wprintf(L"Interface Description[%d]: %S\n", i, pIfInfo->strInterfaceDescription);

        std::cout << "Interface State: " << pIfInfo->isState << std::endl;
        // std::cout << "Interface GUID: " << pIfInfo->InterfaceGuid << std::endl;


        // iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 39); 
        // // For c rather than C++ source code, the above line needs to be
        // // iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 39); 
        // if (iRet == 0)
        //     wprintf(L"StringFromGUID2 failed\n");
        // else {
        //     wprintf(L"  InterfaceGUID[%d]: %ws\n",i, GuidString);
        // }    

        switch (pIfInfo->isState) {
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

    if (pIfList != NULL) {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }
    return 0;
}
