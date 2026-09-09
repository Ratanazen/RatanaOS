#include <stdio.h>
#include <unistd.h>

int main(void) {
    // Standard ANSI Clear Screen and Reset Cursor
    printf("\033[2J\033[H");
    return 0;
}
