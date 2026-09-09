#include <stdio.h>
#include <unistd.h>

int main(void) {
    printf("SHELL=/bin/sh\n");
    printf("PATH=/bin:/sbin:/usr/bin\n");
    printf("USER=root\n");
    printf("HOME=/root\n");
    printf("TERM=xterm-256color\n");
    printf("OS=RatanaOS-x86_64\n");
    return 0;
}
