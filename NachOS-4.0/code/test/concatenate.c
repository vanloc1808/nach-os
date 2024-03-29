#include "syscall.h"

int main() {
    char filePath1[256];
    char filePath2[256];
    char buffer[256];
    int count;
    int fd1;
    int fd2;
    int pos;

    PrintString("Filepath 1: ");
    ReadString(filePath1, 255); // Read from console!
    filePath1[255] = 0; // Nullbyte for last char

    PrintString("Filepath 2: ");
    ReadString(filePath2, 255); // Read from console!
    filePath2[255] = 0; // Nullbyte for last char

    fd1 = Open(filePath1, 1); // Write to end of file 1
    pos = Seek(-1, fd1);
    fd2 = Open(filePath2, 0);
    if (fd1 != -1 && fd2 != -1 && pos != -1) {

        while ((count = Read(buffer, 256, fd2)) > 0) {
            Write(buffer, count, fd1);
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