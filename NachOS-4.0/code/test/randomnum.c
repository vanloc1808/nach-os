/* randomnum.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a randomnum syscall that make a random number.
 *
 */

#include "syscall.h"

int main()
{
    int t;
    t = RandomNum();
    t = RandomNum();
    Halt();
  /* not reached */
}
