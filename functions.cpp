#include <iostream>
#include <cstdio>
#include <stdbool.h>
#include "main.h"
#include <WinSock2.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <string>
#include <vector>
#pragma comment(lib, "iphlpapi.lib")



void set_DNS_server(bool activate, bool is_wifi) {
    ULONG size = 0;
    GetAdaptersAddresses(AF_INET, 0, NULL, NULL, &size);

    IP_ADAPTER_ADDRESSES* ip_adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(malloc(size));
    DWORD res = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, ip_adapters, &size);
    if (res != NO_ERROR) {
        printf("Could'nt set DNS-Server\n");
        return;
    }
    DWORD i = 0;
    for (IP_ADAPTER_ADDRESSES* crnt = ip_adapters; crnt; crnt = crnt->Next) {
        if (crnt->OperStatus == IfOperStatusUp && crnt->IfType != IF_TYPE_SOFTWARE_LOOPBACK && crnt->IfType == IF_TYPE_ETHERNET_CSMACD && !is_wifi && crnt->FirstGatewayAddress != nullptr){
            i = crnt->IfIndex; 
            break;
        }
        else if (crnt->OperStatus == IfOperStatusUp && crnt->IfType != IF_TYPE_SOFTWARE_LOOPBACK && crnt->IfType == IF_TYPE_IEEE80211 && is_wifi){
            i = crnt->IfIndex; 
            break;
        }
    }
    free(ip_adapters);
    if (i == 0) {
        printf("Could'nt set DNS-Server\n");
        return;
    }
    std::string cmd = "";
    if (activate) cmd = "netsh interface ipv4 set dnsservers name=" + std::to_string(i) + " static 127.0.0.1";
    else cmd = "netsh interface ipv4 set dnsservers name=" + std::to_string(i) + " dhcp";
    
    std::cout << cmd << std::endl;
    std::vector<char> command(cmd.begin(), cmd.end());
    command.push_back('\0');
    STARTUPINFOA start_info = { sizeof(start_info) };
    PROCESS_INFORMATION process_info;

    if (!CreateProcessA(NULL, command.data(), NULL, NULL, false, CREATE_NO_WINDOW, NULL, NULL, &start_info, &process_info)) {
        printf("Could'nt set DNS-Server\n");
        return;
    }
    WaitForSingleObject(process_info.hProcess, INFINITE);
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    return;
}