#include "../../libc/include/unistd.h"
#include "../../libc/include/stdio.h"
#include "../../libc/include/stdlib.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    printf("EXECVE SUCCESS: Hello from /bin/hello userspace process! (PID: %d, PPID: %d)\n", (int)getpid(), (int)getppid());
    return 0;
}
