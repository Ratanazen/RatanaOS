#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    printf("1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536\n");
    printf("    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00\n");
    printf("    inet 127.0.0.1/8 scope host lo\n");
    printf("2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 (Intel e1000)\n");
    printf("    link/ether 52:54:00:12:34:56 brd ff:ff:ff:ff:ff:ff\n");
    printf("    inet 10.0.2.15/24 brd 10.0.2.255 scope global eth0\n");
    printf("    gateway 10.0.2.2\n");
    return 0;
}
