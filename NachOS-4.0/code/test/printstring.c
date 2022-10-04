/* readchar.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that read a character from console.
 *
 */

#include "syscall.h"

int main()
{
    char const* c = "This is not a string!";
    PrintString(c);
    Halt();
  /* not reached */
}
