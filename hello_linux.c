#include <unistd.h>
int main() {
    write(1, "Hello from Linux ELF on RiOS\n", 33);
    return 0;
}
