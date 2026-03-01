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
        while (true) {
            ; //hier kommt die server schleife
        }
    }

    system("pause");
    closesocket(mainsocket);
    WSACleanup();
    return 0;
}