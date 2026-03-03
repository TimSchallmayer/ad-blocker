#pragma once
#include <iostream>
#include <cstdio>
#include <stdbool.h>
#include <string>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

//structs
struct DNS_HEADER {
    unsigned short id;
    unsigned short flags;
    unsigned short qcount;
    unsigned short ans_count;
    unsigned short authcount;
    unsigned short addcount;
    bool is_anfrage;
};
struct DNS_body {
    bool is_web;
    const char* qname;
    unsigned short qclass;
};

// funtctions
void set_DNS_server(bool activate, bool is_wifi);
// setzt die DNS server einstellung für Windows auf localhost oder auf DHCP, sollte activate false sein. is_wifi entscheidet ob der Wlan Adapter umgestellt wird das Ethernet.
SOCKET create_socket(int port);
// erstellt einen Socket auf dem angegebenen Port und gibt diesen zurück. Bei Fehlern wird INVALID_SOCKET zurückgegeben.
DNS_HEADER process_packets(char* speicher, int bytes);
// verarbetiett die header der pakete die der dns server empfängt
DNS_body parse_dns_packet(char* speicher, DNS_HEADER header);