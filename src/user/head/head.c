#include <stdio.h>
#include <unistd.h>

static void head_stream(int fd, int max_lines) {
    char buf[512];
    int n;
    int lines = 0;

    while (lines < max_lines && (n = read(fd, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n && lines < max_lines; i++) {
            putchar(buf[i]);
            if (buf[i] == '\n') {
                lines++;
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc == 1) {
        head_stream(0, 10);
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], 0);
        if (fd < 0) {
            printf("head: cannot open '%s'\n", argv[i]);
            continue;
        }
        head_stream(fd, 10);
        close(fd);
    }
    return 0;
}
