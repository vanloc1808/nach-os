 /* sort.c 
    *    Test program to sort a large number of integers.
    *
    *    Intention is to stress virtual memory system.
    *
    *    Ideally, we could read the Dunsorted array off of the file system,
    *	and store the result back to the file system!
 */

#include "syscall.h"

void swapElements(int a[], int i, int j) {
    int temp = a[i];
    a[i] = a[j];
    a[j] = temp;
}

void sortAscending(int a[], int n) {
    int i, j, temp;
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (a[i] > a[j]) {
                swapElements(a, i, j);
            }
        }
    }
}

void sortDescending(int a[], int n) {
    int i, j, temp;
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (a[i] < a[j]) {
                swapElements(a, i, j);
            }
        }
    }
}

int main() {
    int n;
    int a[100];
    int i;
    int order;

    PrintString("Enter a positive integer n\n");
    n = ReadNum();

    while (n <= 0 || n > 100 ) {
        PrintString("Please enter n in [1, 100]\n");
        n = ReadNum();
    }

    for (i = 0; i < n; i++) {
        PrintString("Enter a[");
        PrintNum(i);
        PrintString("]\n");

        a[i] = ReadNum();
    }

    PrintString("Enter 0 for ascending order, 1 for descending order\n");
    order = ReadNum();

    if (order == 0) {
        sortAscending(a, n);
    } else {
        sortDescending(a, n);
    }

    PrintString("After sorting in ");
    if (order == 0) {
        PrintString("ascending order\n");
    } else {
        PrintString("descending order\n");
    }
    for (i = 0; i < n; i++) {
        PrintNum(a[i]);
        PrintString(" ");
    }
    PrintChar('\n');

    Halt();
}