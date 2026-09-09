#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int fd = 0;
    if (argc > 1) {
        fd = open(argv[1], 0);
        if (fd < 0) {
            printf("tail: cannot open '%s'\n", argv[1]);
            return 1;
        }
    }

    char buf[4096];
    int n = read(fd, buf, sizeof(buf) - 1);
    if (fd > 0) close(fd);

    if (n <= 0) return 0;
    buf[n] = '\0';

    int newline_count = 0;
    for (int i = 0; i < n; i++) {
        if (buf[i] == '\n') newline_count++;
    }

    int target_skip = (newline_count > 10) ? (newline_count - 10) : 0;
    int current_lines = 0;
    int start_pos = 0;

    for (int i = 0; i < n; i++) {
        if (current_lines >= target_skip) {
            start_pos = i;
            break;
        }
        if (buf[i] == '\n') current_lines++;
    }

    printf("%s", buf + start_pos);
    return 0;
}
