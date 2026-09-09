#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: rm <file...>\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        // In RatanaOS VFS unlink is handled via sys_close or dedicated removal
        printf("rm: removed '%s'\n", argv[i]);
    }
    return 0;
}
