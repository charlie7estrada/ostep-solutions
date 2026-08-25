// 4. Write a simple program that allocates memory using malloc() but
// forgets to free it before exiting. What happens when this program
// runs? Can you use gdb to find any problems with it? How about
// valgrind (again with the --leak-check=yes flag)?

#include <stdio.h>
#include <stdlib.h>

int main(){

    int *ptr = malloc(sizeof(int));
    *ptr = 42;
    printf("%d\n", *ptr);

}

// gcc leak.c -o leak
// ./leak

// Result: Program ran fine. Returns 42.

// gcc leak.c -o leak -g
// gdb ./leak
// run (then quit when done)

// gdb does not produce anything. no errors.

// valgrind --leak-check=yes ./leak

// valgrind correctly caught it: "4 bytes in 1 blocks are definitely lost" - pinpointing the exact malloc() call at leak.c:11 responsible. 
// HEAP SUMMARY confirms 2 allocs, 1 free (the missing free is the leak).