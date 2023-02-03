#include "syscall.h"

void assert(int condition, const char* message) {
    if (condition == 0) {
        PrintString(message);
        Halt();
    }
}
char buf[] = "123 456\n789\tabc\\def";

void phase1() {
    int fd;
    int x;
    int fd_test;
    int del;

    fd = Open("file1.txt", 0);
    assert(fd == -1, "File already");
    x = Create("file1.txt");
    assert(x >= 0, "Cannot create file");

    fd = Open("file1.txt", 1);
    del = Remove("file1.txt");
    assert(del == -1, "Error deleting file");
    assert(fd == 2, "Cannot open");
    fd_test = Open("file1.txt", 1);
    PrintNum(fd_test);
    PrintChar('\n');
    //assert(fd_test == -1, "Hello");
    assert(Write(buf, 20, fd) > 0, "Write not successful");
    assert(Write(buf, 20, 3) == -1, "Write to non-opened file unexpected");
    assert(Close(fd) != -1, "close fail");
    assert(Close(3) == -1, "Close non-opened file unexpected");
    PrintString("\n Phase1 done!\n");
}

void phase2() {
    int fd;
    int x;
    char readBuf[64];
    fd = Open("file1.txt", 0);
    PrintNum(fd);
    assert(fd == 2, "Not take prev fd");
    assert(Write(buf, 20, 2) == -1, "Write to read-only file!");
    x = Read(readBuf, 20, fd);
    assert(x == 20, "Read not equal 20 chars");
    Write(readBuf, 20, 1);
    x = Seek(2, fd);
    assert(x == 2, "Seek not at pos 2");
    x = Read(readBuf, 10, fd);
    assert(x == 10, "Read not equal 10 chars");
    PrintChar('\n');
    Write(readBuf, 10, 1);
    Close(fd);
    PrintString("\n Phase2 done!\n");
}

void phase3() {
    int fd;
    char readBuf[64];
    Remove("file1.txt");
    assert(Open("file1.txt", 0) == -1, "Open removed file!");
    Create("test.txt");
    fd = Open("test.txt", 1);
    assert(fd == 2, "fd not take back");
    Write(buf, 10, fd);
    Seek(3, fd);
    Write(buf, 7, fd);
    Read(readBuf, 5, fd);
    Write(readBuf, 5, 1);
    Seek(-1, fd);
    Write(buf, 20, fd);
    Seek(0, fd);
    Read(readBuf, 64, fd);
    Close(fd);
    PrintString("\n Phase3 done!\n");
}

int main() {
    PrintString("Phase1:\n");
    phase1();
    PrintString("Phase2:\n");
    phase2();

    PrintString("Phase3:\n");
    phase3();
    
    Halt();
}