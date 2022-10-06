#include "syscall.h"

int main() {
    unsigned char c = (unsigned char)(0);

    while (c <= (unsigned char)(255)) {
        PrintNum((int)c);
        PrintString(": ");
        PrintChar(c);

        PrintChar('\n');
        c++;
    }

    Halt();
}