#include "syscall.h"

int main() {
    char filePath[256];
    char buffer[256];
    int count = 0;
    int fd = -1;
    PrintString("Filepath: ");
    
    ReadString(filePath, 255); // Read from console!
    filePath[255] = 0; // Nullbyte for last char

    fd = Open(filePath, 0);
    
    if (fd != -1) {
        while ((count = Read(buffer, 256, fd)) > 0) {
            Write(buffer, count, 1);
        }

        Close(fd);
    } else {
        PrintString("Unable to open file\n");
    }

    PrintChar('\n');

    Halt();
}