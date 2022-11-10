#include "syscall.h"

int main(int argc, char** argv) {
    if (argc == 2) {
        int res = CreateFile(argv[1]);
        if (res == 0) {
            PrintString("Create file successfully!\n");
        } else {
            PrintString("Create file failed!\n");
        }
    } else if (argc == 1) {
        PrintString("Enter file name to create: \n");
        char* fileName = ReadString();
        int res = CreateFile(fileName);
        if (res == 0) {
            PrintString("Create file successfully!\n");
        } else {
            PrintString("Create file failed!\n");
        }
    }
    Halt();
}