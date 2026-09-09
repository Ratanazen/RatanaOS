#include "../../libc/include/unistd.h"
#include "../../libc/include/stdio.h"
#include "../../libc/include/stdlib.h"
#include "../../libc/include/string.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: cat <file>\n");
        return 1;
    }

    int fd = open(argv[1], 0);
    if (fd < 0) {
        printf("cat: %s: No such file or directory\n", argv[1]);
        return 1;
    }

    char buffer[512];
    int64_t bytes;
    while ((bytes = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    close(fd);
    return 0;
}
