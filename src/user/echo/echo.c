#include "../../libc/include/unistd.h"
#include "../../libc/include/stdio.h"

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        printf("%s%s", argv[i], (i == argc - 1) ? "" : " ");
    }
    putchar('\n');
    return 0;
}
