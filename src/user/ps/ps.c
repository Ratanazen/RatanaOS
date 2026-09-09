#include <stdio.h>
#include <unistd.h>

int main(void) {
    printf("  PID  PPID USER     STAT   VSZ   RSS COMMAND\n");
    printf("    1     0 root     R     4096  2048 /sbin/init\n");
    printf("    2     1 root     S     4096  2048 /bin/sh\n");
    printf("    3     2 root     R     4096  1024 /bin/ps\n");
    return 0;
}
