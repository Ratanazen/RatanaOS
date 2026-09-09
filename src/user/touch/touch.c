#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: touch <file...>\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], 0x42); // O_RDWR | O_CREAT
        if (fd < 0) {
            printf("touch: cannot touch '%s'\n", argv[i]);
            return 1;
        }
        close(fd);
    }
    return 0;
}
