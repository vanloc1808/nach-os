#include "syscall.h"

int main() {
    char fileName[1000];
    int res = 1;
        PrintString("Please enter file name for creating\n");
        ReadString(fileName, 1000);
        PrintString("... Creating file...\n");

        res = Create(fileName);

        if (res == 0) {
            PrintString("... File created successfully\n");
        } else {
            PrintString("... Error creating file\n");
        }
        
        PrintString(fileName);
        PrintChar('\n');
        PrintString("End of function\n"); 
    Halt();
}