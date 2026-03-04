#include "main.h"
#include <Windows.h>
#include <vector>
using namespace std;

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
    string cmd = "";
    if (activate) cmd = "netsh interface ipv4 set dnsservers name=" + to_string(i) + " static 127.0.0.1";
    else cmd = "netsh interface ipv4 set dnsservers name=" + to_string(i) + " dhcp";
    
    cout << cmd << endl;
    vector<char> command(cmd.begin(), cmd.end());
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

SOCKET create_recv_socket(int port) {
    
    SOCKET sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in svr_adr;
    svr_adr.sin_port = htons(port);
    svr_adr.sin_family = AF_INET;
    svr_adr.sin_addr.s_addr = INADDR_ANY;

    bind(sckt, (struct sockaddr*)&svr_adr, sizeof(svr_adr));

    return sckt;
}
SOCKET create_send_socket(int port) {
    SOCKET sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    return sckt;

}
DNS_HEADER process_packets_header(unsigned char* speicher, int bytes) {
    //packet auslesen
    if (bytes < 12) return {NULL, NULL, NULL, NULL, NULL, NULL, NULL}; //zuklein

    unsigned short id = ntohs(*(unsigned short*)(speicher));
    unsigned short flags = ntohs(*(unsigned short*)(speicher + 2));
    unsigned short qcount = ntohs(*(unsigned short*)(speicher + 4));
    bool is_anfrage = (flags & 0x8000);
    cout << "Paket empfagen. ID: " << id << " Typ: " << (is_anfrage ? " Anfrage " : " Antwort ")<< " Anzahl der Adressen: " << qcount << endl;
    DNS_HEADER header;
    header.id = id;
    header.flags = flags;
    header.qcount = qcount;
    header.addcount = 0;
    header.ans_count = 0;
    header.authcount = 0;
    header.is_anfrage = is_anfrage;
    return header;
}
DNS_body parse_dns_packet(unsigned char* speicher, DNS_HEADER header, int recv_bytes) {
    if (header.qcount <= 0) return {NULL, NULL, NULL};
    int offset = 12;
    string name = "";
    int lenght = 0;
    for (; offset < recv_bytes && speicher[offset] != 0x00 ; offset++) { 
        if (lenght == 0) {
            lenght = speicher[offset];
            if (offset != 12) name += '.';
        }
        else if (lenght > 0) {
            lenght -= 1;
            name += speicher[offset];
            }
    }  
    if (offset +1 > recv_bytes) return {NULL, NULL, NULL};

    offset += 1;
    unsigned short tmp;
    memcpy(&tmp, speicher + offset, sizeof(unsigned short));
    bool is_web = ntohs(tmp) == 1;
    if (offset + 2 > recv_bytes) return {NULL, NULL, NULL};
    offset += 2;
    memcpy(&tmp, speicher + offset, sizeof(unsigned short));
    unsigned short qclass = ntohs(tmp);


    if (qclass != 0x01) return {NULL, NULL, NULL};
    cout << "Erfolgreich dns packet geparst. Name: " << name << " Web?:" << is_web << " Klasse: " << qclass << endl;
    return {is_web, name, qclass};
}
void skipforward(char * speicher, int len, SOCKET sckt) {
    struct sockaddr_in svr_adr;
    svr_adr.sin_family = AF_INET;
    inet_pton(AF_INET, "8.8.8.8", &svr_adr); // schlechte lösung weil das hier ja eigentlich ausfallen könnte und man sollte am besten merhere ooptionen haben
    sendto(sckt, speicher, len, 0, (sockaddr*)&svr_adr, sizeof(svr_adr));
    return;
}