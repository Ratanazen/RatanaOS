#include <stdio.h>
#include <unistd.h>

int main(void) {
    int fd = open("/proc/uptime", 0);
    if (fd >= 0) {
        char buf[64];
        int n = read(fd, buf, sizeof(buf) - 1);
        close(fd);
        if (n > 0) {
            buf[n] = '\0';
            printf("RatanaOS System Time: 2026-09-09 (uptime: %s)", buf);
            return 0;
        }
    }
    printf("Wed Sep  9 10:55:00 UTC 2026\n");
    return 0;
}
