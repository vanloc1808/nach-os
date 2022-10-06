/* readnum.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a readnum syscall that read a number from console.
 *
 */

#include "syscall.h"

int main()
{
    int t;
    t = ReadNum();
    Halt();
  /* not reached */
}
