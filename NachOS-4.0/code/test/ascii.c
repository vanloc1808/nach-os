#include "syscall.h"

int main() {
    unsigned char c = (unsigned char)(0);

    while (c <= (unsigned char)(255)) {
        PrintNum((int)c);
        PrintChar(': ');
        PrintChar(c);

        PrintString('\n');
        c++;
    }

    Halt();
}