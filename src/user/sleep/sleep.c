#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: sleep <seconds>\n");
        return 1;
    }

    int sec = atoi(argv[1]);
    if (sec <= 0) sec = 1;

    // Yield loop for sleep approximation
    for (int i = 0; i < sec * 100; i++) {
        yield();
    }
    return 0;
}
