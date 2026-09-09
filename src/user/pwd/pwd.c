#include "../../libc/include/unistd.h"
#include "../../libc/include/stdio.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    char buf[128];
    if (getcwd(buf, sizeof(buf))) {
        printf("%s\n", buf);
        return 0;
    }
    printf("/\n");
    return 0;
}
