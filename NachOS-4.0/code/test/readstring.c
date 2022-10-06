/* readchar.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that read a character from console.
 *
 */

#include "syscall.h"

int main()
{
    char result[1000];
    ReadString(result, 1000);
    Halt();
  /* not reached */
}
