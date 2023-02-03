/* printchar.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a printchar syscall that print a character to console.
 *
 */

#include "syscall.h"

int main()
{
    char c = 'X';
    PrintChar(c);
    Halt();
  /* not reached */
}
