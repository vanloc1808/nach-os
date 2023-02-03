/* printstring.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a printstring syscall that print a string to console.
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
