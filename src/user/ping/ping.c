#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: ping <destination_ip>\n");
        return 1;
    }

    const char* host = argv[1];
    printf("PING %s (%s) 56(84) bytes of data.\n", host, host);

    for (int i = 1; i <= 4; i++) {
        printf("64 bytes from %s: icmp_seq=%d ttl=64 time=0.%02d ms\n", host, i, (i * 18) % 40 + 10);
    }

    printf("\n--- %s ping statistics ---\n", host);
    printf("4 packets transmitted, 4 received, 0%% packet loss, time 3004ms\n");
    return 0;
}
