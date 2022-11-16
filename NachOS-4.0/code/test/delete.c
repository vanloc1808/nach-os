#include "syscall.h"

int main() {
    char fileName[1000];
    int res = 1;
    PrintString("Please enter file name for deleting\n");
    ReadString(fileName, 1000);
    PrintString("... Deleting file... ");
    PrintString(fileName);
    PrintChar('\n');

    res = Remove(fileName);
    PrintNum(res);
    PrintChar('\n');

    if (res == 0) {
        PrintString("... File deleted successfully\n");
    } else {
        PrintString("... Error deleting file\n");
    }
    
    Halt();
}