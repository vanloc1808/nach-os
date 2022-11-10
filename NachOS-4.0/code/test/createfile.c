#include "syscall.h"

int main() {
    char fileName[1000];
    PrintString("Please enter file name for creating\n");
    ReadString(fileName, 1000);
    // PrintString("... Creating file ");
    
    PrintString(fileName);
    // PrintChar('\n');
    // PrintString("End of function\n");
    
    Halt();
}