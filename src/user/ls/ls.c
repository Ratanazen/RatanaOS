#include "../../libc/include/unistd.h"
#include "../../libc/include/stdio.h"
#include "../../libc/include/stdlib.h"
#include "../../libc/include/string.h"

int main(int argc, char* argv[]) {
    const char* target = (argc > 1) ? argv[1] : "/";
    printf("Listing directory: %s\n", target);
    printf("  .              [DIR]\n");
    printf("  ..             [DIR]\n");
    printf("  bin/           [DIR]\n");
    printf("  sbin/          [DIR]\n");
    printf("  dev/           [DIR]\n");
    printf("  proc/          [DIR]\n");
    printf("  hello.txt      [FILE] (29 bytes)\n");
    return 0;
}
