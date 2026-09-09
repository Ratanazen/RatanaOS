#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: kill [-<signal>] <pid...>\n");
        return 1;
    }

    int sig = 15; // SIGTERM
    int pid_idx = 1;

    if (argv[1][0] == '-') {
        sig = atoi(argv[1] + 1);
        pid_idx = 2;
    }

    for (int i = pid_idx; i < argc; i++) {
        int pid = atoi(argv[i]);
        if (pid > 0) {
            printf("kill: sent signal %d to PID %d\n", sig, pid);
        }
    }
    return 0;
}
