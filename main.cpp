#include "main.h"

using namespace std;

int main() {
    system("powershell -Command \"[Console]::OutputEncoding = [System.Text.Encoding]::UTF8\""); //utf8 terminal
  //  printf("Hallo Welt!\n"); 
    set_DNS_server(true, true);
    set_DNS_server(false, false);
    vector<string> dns_adrrss = {"9.9.9.9", "8.8.8.8", "1.1.1.1"};
    unordered_set<string> liste = lesen("block_liste.txt");
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    SOCKET mainsocket = create_recv_socket(53);
    SOCKET send_socket = create_send_socket(53);
    if (mainsocket != INVALID_SOCKET && send_socket != INVALID_SOCKET) {
        printf("DNS_Server ist bereit auf port 53...\n");
        unsigned char speicher[512];
        sockaddr_in user_adrr;
        int user_adrr_len = sizeof(user_adrr);
        while (true) {
            //hier kommt die server schleife
            int recvbytes = recvfrom(mainsocket, (char *)speicher, sizeof(speicher), 0, (struct sockaddr*)&user_adrr, &user_adrr_len);
            if (recvbytes == SOCKET_ERROR)
            {
        //        cout << "Error recieving the packages." << "Error: " << WSAGetLastError() << endl;
                continue;
            }
            char user_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &user_adrr.sin_addr, user_ip, INET_ADDRSTRLEN);
           // cout << "Paket von " << user_ip << " erhalten (" << recvbytes << " Bytes)." << endl;
            DNS_HEADER packet_header = process_packets_header(speicher, recvbytes); 
            DNS_body packet_body = parse_dns_packet(speicher, packet_header, recvbytes);
            if (packet_body.is_web == NULL) {
         //       cout << "ERROR beim parsen";
                continue;
             } //skip
            //zum test erstmal alle weiter leiten
            if (liste.count(packet_body.qname))
            {
                cout << "DOMAIN BLOCKED" << endl;
                continue;
            }
            else {
                cout << "DOMAIN PASSED" << endl;
                cout << "DOMAIN: " << packet_body.qname << endl;
                skipforward((char*)speicher, recvbytes, send_socket, dns_adrrss, 0, user_adrr, user_adrr_len, mainsocket);
            }
            
        }   
    }

    system("pause");
    closesocket(mainsocket);
    WSACleanup();
    return 0;
}