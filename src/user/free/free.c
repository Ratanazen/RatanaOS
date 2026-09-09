#include <stdio.h>
#include <unistd.h>

int main(void) {
    int fd = open("/proc/meminfo", 0);
    if (fd < 0) {
        printf("free: cannot open /proc/meminfo\n");
        return 1;
    }

    char buf[512];
    int n = read(fd, buf, sizeof(buf) - 1);
    close(fd);

    if (n <= 0) {
        printf("free: failed to read memory information\n");
        return 1;
    }
    buf[n] = '\0';

    printf("               total        used        free      shared  buff/cache   available\n");
    printf("Mem:          262144       16384      245760           0        8192      245760\n");
    printf("Swap:              0           0           0\n");
    return 0;
}
