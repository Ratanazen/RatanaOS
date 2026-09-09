#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINES 128
#define MAX_LEN   128

static char lines[MAX_LINES][MAX_LEN];
static int num_lines = 0;

int main(int argc, char** argv) {
    int fd = 0;
    if (argc > 1) {
        fd = open(argv[1], 0);
        if (fd < 0) {
            printf("sort: cannot open '%s'\n", argv[1]);
            return 1;
        }
    }

    char buf[1024];
    int n;
    int line_idx = 0;

    while ((n = read(fd, buf, sizeof(buf))) > 0 && num_lines < MAX_LINES) {
        for (int i = 0; i < n && num_lines < MAX_LINES; i++) {
            if (buf[i] == '\n') {
                lines[num_lines][line_idx] = '\0';
                num_lines++;
                line_idx = 0;
            } else if (line_idx < MAX_LEN - 1) {
                lines[num_lines][line_idx++] = buf[i];
            }
        }
    }
    if (line_idx > 0 && num_lines < MAX_LINES) {
        lines[num_lines][line_idx] = '\0';
        num_lines++;
    }
    if (fd > 0) close(fd);

    // Bubble sort lines
    for (int i = 0; i < num_lines - 1; i++) {
        for (int j = 0; j < num_lines - i - 1; j++) {
            if (strcmp(lines[j], lines[j + 1]) > 0) {
                char tmp[MAX_LEN];
                strcpy(tmp, lines[j]);
                strcpy(lines[j], lines[j + 1]);
                strcpy(lines[j + 1], tmp);
            }
        }
    }

    for (int i = 0; i < num_lines; i++) {
        printf("%s\n", lines[i]);
    }
    return 0;
}
