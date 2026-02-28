#include <cstdio>
#include <iostream>
#include "main.h"
#include <stdbool.h>

using namespace std;

int main() {
    printf("Hallo Welt!\n"); 
    set_DNS_server(false, true);
    set_DNS_server(false, false);
    system("pause"); 
    return 0;
}