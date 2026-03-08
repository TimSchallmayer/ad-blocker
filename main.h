#pragma once
#include <iostream>
#include <cstdio>
#include <stdbool.h>
#include <string>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <vector>
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
    std::string qname;
    unsigned short qclass;
};

// funtctions
void set_DNS_server(bool activate, bool is_wifi);
// setzt die DNS server einstellung für Windows auf localhost oder auf DHCP, sollte activate false sein. is_wifi entscheidet ob der Wlan Adapter umgestellt wird das Ethernet.
SOCKET create_recv_socket(int port);
// erstellt einen Socket, der auf Datenpakete wartet auf dem angegebenen Port und gibt diesen zurück. Bei Fehlern wird INVALID_SOCKET zurückgegeben.
DNS_HEADER process_packets_header(unsigned char* speicher, int bytes);
// verarbetiett die header der pakete die der dns server empfängt
DNS_body parse_dns_packet( unsigned char* speicher, DNS_HEADER header, int recv_bytes);
// verarbeitet den body der packet die der dns server empfängt
SOCKET create_send_socket(int port);
// erstellt einen Socket der pakte zum DNS server sendet auf dem angegebenen Port und gibt diesen zurück. Bei Fehlern wird INVALID_SOCKET zurückgegeben.
void skipforward(char * speicher, int len, SOCKET sckt, std::vector<std::string> dns_adrrss, int index, sockaddr_in user_addr, int addr_len, SOCKET main_socket);
// sendet das packet an einen dns server