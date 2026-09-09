#include <stdio.h>
#include <unistd.h>

static void wc_stream(int fd, const char* name) {
    char buf[1024];
    int n;
    int lines = 0, words = 0, bytes = 0;
    int in_word = 0;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        bytes += n;
        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n') lines++;
            if (buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\n' || buf[i] == '\r') {
                in_word = 0;
            } else if (!in_word) {
                in_word = 1;
                words++;
            }
        }
    }

    if (name) {
        printf(" %d  %d  %d %s\n", lines, words, bytes, name);
    } else {
        printf(" %d  %d  %d\n", lines, words, bytes);
    }
}

int main(int argc, char** argv) {
    if (argc == 1) {
        wc_stream(0, NULL);
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], 0);
        if (fd < 0) {
            printf("wc: cannot open '%s'\n", argv[i]);
            continue;
        }
        wc_stream(fd, argv[i]);
        close(fd);
    }
    return 0;
}
