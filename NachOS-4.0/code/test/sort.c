/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */


/*
#define UNIX
#define UNIX_DEBUG
*/

#ifdef UNIX
#include <stdio.h>
#define Exit exit
#else
#include "syscall.h"
#endif /* UNIX */

#define SIZE (1024)

int A[SIZE];	/* size of physical memory; with code, we'll run out of space!*/

int
main()
{
    int i, j, tmp, n;

    PrintString("Size of your array: ");
    n = ReadNum();

    if (n > SIZE) {
        PrintString("Array too big!\n");
        Halt();
    }

    /* first initialize the array, in reverse sorted order */
    for (i = 0; i < n; i++) {
        PrintString("Enter number ");
        PrintNum(i);
        PrintString(": ");

        A[i] = ReadNum();
    }

    /* then sort! */
    for (i = 0; i < n; i++) {
        for (j = 0; j < (n-1); j++) {
	   if (A[j] > A[j + 1]) {	/* out of order -> need to swap ! */
	      tmp = A[j];
	      A[j] = A[j + 1];
	      A[j + 1] = tmp;
    	   }
        }
    }

#ifdef UNIX_DEBUG
    for (i=0; i<SIZE; i++) {
        printf("%4d ", A[i]);
	if (((i+1) % 15) == 0) {
		printf("\n");
        }
        if (A[i] != i) {
            fprintf(stderr, "Out of order A[%d] = %d\n", i, A[i]);
            Exit(1);
        }   
    }
    printf("\n");
#endif /* UNIX_DEBUG */

    for (i=0; i<n; i++)
    {
        PrintString("A[");
        PrintNum(i);
        PrintString("] = ");
        PrintNum(A[i]);
        PrintChar('\n');
    }

    Halt();
}
