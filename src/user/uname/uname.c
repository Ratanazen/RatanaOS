#include "../../libc/include/unistd.h"
#include "../../libc/include/stdio.h"
#include "../../libc/include/sys/utsname.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    struct utsname u;
    if (uname(&u) == 0) {
        printf("%s %s %s %s %s\n", u.sysname, u.nodename, u.release, u.version, u.machine);
        return 0;
    }
    printf("RatanaOS x86_64\n");
    return 1;
}
