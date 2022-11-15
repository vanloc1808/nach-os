#include "syscall.h"

int main() {
    char filePath1[256];
    char filePath2[256];
    char buffer[1024];
    int count;

    PrintString("Source: ");
    ReadString(filePath1, 255); // Read from console!
    filePath1[255] = 0; // Nullbyte for last char

    PrintString("Destination: ");
    ReadString(filePath2, 255); // Read from console!
    filePath2[255] = 0; // Nullbyte for last char

    int fd1 = Open(filePath1, 0);
    int fd2 = Open(filePath2, 1);
    if (fd1 != -1 && fd2 != -1) {
        
        while (count = Read(buffer, 1024, fd1) > 0) {
            Write(buffer, count, fd2);
        }

        Close(fd1);
        Close(fd2);
    } else {
        if (fd1 != -1) Close(fd1);
        if (fd2 != -1) Close(fd2);
        PrintString("Unable to open file\n");
    }

    Halt();
}