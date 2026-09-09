#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: mkdir <directory...>\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], 0x42);
        if (fd < 0) {
            printf("mkdir: cannot create directory '%s'\n", argv[i]);
            return 1;
        }
        close(fd);
    }
    return 0;
}
