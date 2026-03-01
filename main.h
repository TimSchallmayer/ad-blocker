#pragma once
#include <iostream>
#include <cstdio>
#include <stdbool.h>
#include <string>
#include <WinSock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

// funtctions
void set_DNS_server(bool activate, bool is_wifi);
// setzt die DNS server einstellung für Windows auf localhost oder auf DHCP, sollte activate false sein. is_wifi entscheidet ob der Wlan Adapter umgestellt wird das Ethernet.
SOCKET create_socket(int port);
// erstellt einen Socket auf dem angegebenen Port und gibt diesen zurück. Bei Fehlern wird INVALID_SOCKET zurückgegeben.