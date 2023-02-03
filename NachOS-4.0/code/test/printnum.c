/* printnum.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a printnum syscall that print a print to console.
 *
 */

#include "syscall.h"

int main()
{
    int x = 1234;
    int y = -2234;
    PrintNum(x);
    PrintNum(y);
    Halt();
  /* not reached */
}
