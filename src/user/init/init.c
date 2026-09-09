#include "../../libc/include/unistd.h"
#include "../../libc/include/stdio.h"
#include "../../libc/include/stdlib.h"
#include "../../libc/include/string.h"
#include "../../libc/include/sys/utsname.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    printf("\n=========================================\n");
    printf("   [INIT] RatanaOS Userspace /sbin/init  \n");
    printf("=========================================\n");

    struct utsname u;
    if (uname(&u) == 0) {
        printf("[INIT] Operating System: %s %s (%s)\n", u.sysname, u.release, u.machine);
    }

    printf("[INIT] Spawning test child process...\n");
    int64_t pid = fork();
    if (pid == 0) {
        printf("[INIT-CHILD] Fork successful (PID: %d). Executing /bin/hello...\n", (int)getpid());
        execve("/bin/hello", NULL, NULL);
        printf("[INIT-CHILD] execve failed!\n");
        _exit(1);
    } else if (pid > 0) {
        printf("[INIT] Parent waiting for child (PID: %d)...\n", (int)pid);
        int status = 0;
        waitpid(pid, &status, 0);
        printf("[INIT] Child process reaped successfully (Exit Status: %d).\n", status);
    }

    return 0;
}
