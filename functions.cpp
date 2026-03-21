#include "main.h"
#include <Windows.h>
#include <fstream>
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
    
    SOCKET send_sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in svr_adr;
    svr_adr.sin_port = htons(port);
    svr_adr.sin_family = AF_INET;
    svr_adr.sin_addr.s_addr = INADDR_ANY;

    bind(send_sckt, (struct sockaddr*)&svr_adr, sizeof(svr_adr));

    return send_sckt;
}
SOCKET create_send_socket(int port) {
    SOCKET send_sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    return send_sckt;

}
DNS_HEADER process_packets_header(unsigned char* speicher, int bytes) {
    //packet auslesen
    if (bytes < 12) return {NULL, NULL, NULL, NULL, NULL, NULL, NULL}; //zuklein

    unsigned short id = ntohs(*(unsigned short*)(speicher));
    unsigned short flags = ntohs(*(unsigned short*)(speicher + 2));
    unsigned short qcount = ntohs(*(unsigned short*)(speicher + 4));
    bool is_anfrage = (flags & 0x8000);
  //  cout << "Paket empfagen. ID: " << id << " Typ: " << (is_anfrage ? " Anfrage " : " Antwort ")<< " Anzahl der Adressen: " << qcount << endl;
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
  //  cout << "Erfolgreich dns packet geparst. Name: " << name << " Web?:" << is_web << " Klasse: " << qclass << endl;
    return {is_web, name, qclass};
}
void skipforward(char * speicher, int len, SOCKET send_sckt, vector<string> dns_adrrss, int index, sockaddr_in user_addr, int addr_len, SOCKET main_socket) {
    for (string dns_adrr : dns_adrrss) 
    {
        struct sockaddr_in svr_adr;
        memset(&svr_adr, 0, sizeof(svr_adr));
        svr_adr.sin_family = AF_INET;
        svr_adr.sin_port = htons(53);
        inet_pton(AF_INET, dns_adrr.c_str(), &svr_adr.sin_addr); // schlechte lösung weil das hier ja eigentlich ausfallen könnte und man sollte am besten merhere ooptionen haben
        int result = sendto(send_sckt, speicher, len, 0, (sockaddr*)&svr_adr, sizeof(svr_adr));
        if (result == -1) {
        //    cout << "ERROR beim verschicken" << endl;
            continue;
        } 
     //   cout << "Erfolgreich Bytes versendet: " << len << endl;
        int timeout = 2000;
        setsockopt(send_sckt, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));;

        sockaddr_in recv_addr;
        int recv_addr_size = sizeof(recv_addr);
        unsigned char recv_speicher[1024];

        int recv_bytes = recvfrom(send_sckt, (char *)&recv_speicher, sizeof(recv_speicher), 0, (struct sockaddr*)&recv_addr, &recv_addr_size);
      //  cout << "Antwort Bytes: " << recv_bytes << endl;
        if (recv_bytes == SOCKET_ERROR)
        {
            if (index +1 >= dns_adrrss.size())
            {
                cout << "DNS Servers not reachable" << endl;
                return;
            }
            index += 1;
            continue;
        }
        int res = sendto(main_socket, (char *)recv_speicher, recv_bytes, 0, (struct sockaddr*)&user_addr, addr_len);
        if (res == -1) cout << "Fehler beim senden der antwort" << endl;
    }
    return;
}
unordered_set<string> lesen(string filename) {
    unordered_set<string> liste;
    char buffer[128];
    FILE* datei = fopen(filename.c_str(), "r");
    if (datei == nullptr) return liste;

    //hier lesen implementieren für Übung erst C weg dann C++ weg dann mit sql statt als datei
    while (fgets(buffer, 128, datei) != NULL) { 
     //   cout << buffer << endl;// fread könnte man hier auch nehmen aber fgets ist geeigneter, weil man hier nur Zeile für Zeile liest und byte für byte, also brauct man das hier nicht bzw. es nicht unnötig.
        buffer[strcspn(buffer, "\n")] = 0;
        liste.insert(buffer);
    }
     // finden von domains klappt nicht mal schauen was sich da noch ändern lässt
    return liste;
}
BOOL WINAPI check_quit(DWORD type) {
    switch (type) {
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
        case CTRL_C_EVENT:

            set_DNS_server(false, true);
            set_DNS_server(false, false);

            return TRUE;
        default:
            return FALSE;
    }
}