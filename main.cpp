#include "main.h"

using namespace std;

int main() {
    printf("Hallo Welt!\n"); 
    set_DNS_server(false, true);
    set_DNS_server(false, false);

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

    SOCKET mainsocket = create_socket(53);
    if (mainsocket != INVALID_SOCKET) {
        printf("DNS_Server ist bereit auf port 53...");
        char speicher[512];
        sockaddr_in user_adrr;
        int user_adrr_len = sizeof(user_adrr);
        while (true) {
            //hier kommt die server schleife
            int recvbytes = recvfrom(mainsocket, speicher, sizeof(speicher), 0, (struct sockaddr*)&user_adrr, &user_adrr_len);
            if (recvbytes == SOCKET_ERROR)
            {
                cout << "Error recieving the packages." << "Error: " << WSAGetLastError() << endl;
                return 1;
            }
            char user_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &user_adrr.sin_addr, user_ip, INET_ADDRSTRLEN);
            cout << "Paket von " << user_ip << " erhalten (" << recvbytes << " Bytes)." << endl;
            
        }   
    }

    system("pause");
    closesocket(mainsocket);
    WSACleanup();
    return 0;
}