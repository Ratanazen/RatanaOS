#include <stdio.h>
#include <unistd.h>
#include <string.h>

static int strpos(const char* haystack, const char* needle) {
    if (!*needle) return 0;
    for (int i = 0; haystack[i]; i++) {
        int match = 1;
        for (int j = 0; needle[j]; j++) {
            if (!haystack[i + j] || haystack[i + j] != needle[j]) {
                match = 0;
                break;
            }
        }
        if (match) return i;
    }
    return -1;
}

static void grep_stream(int fd, const char* pattern) {
    char buf[1024];
    int n;
    char line[256];
    int line_idx = 0;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                line[line_idx] = '\0';
                if (strpos(line, pattern) >= 0) {
                    printf("%s\n", line);
                }
                line_idx = 0;
            } else if (line_idx < 255) {
                line[line_idx++] = buf[i];
            }
        }
    }
    if (line_idx > 0) {
        line[line_idx] = '\0';
        if (strpos(line, pattern) >= 0) {
            printf("%s\n", line);
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: grep <pattern> [file...]\n");
        return 1;
    }

    const char* pattern = argv[1];

    if (argc == 2) {
        // Read from stdin (fd 0)
        grep_stream(0, pattern);
        return 0;
    }

    for (int i = 2; i < argc; i++) {
        int fd = open(argv[i], 0);
        if (fd < 0) {
            printf("grep: %s: No such file or directory\n", argv[i]);
            continue;
        }
        grep_stream(fd, pattern);
        close(fd);
    }
    return 0;
}
