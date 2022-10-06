/* help.c
 *	Simple program to show information of group.
 *	
 *	Just do printstring syscalls.
 *
 */

#include "syscall.h"

int main()
{
    PrintString((char*)"Thanh vien trong nhom:\n");
    PrintString((char*)"1. 20120131 - Nguyen Van Loc\n");
    PrintString((char*)"2. 20120536 - Vo Trong Nghia\n");
    PrintString((char*)"3. 20120572 - Nguyen Kieu Minh Tam\n");
    Halt();
  /* not reached */
}
