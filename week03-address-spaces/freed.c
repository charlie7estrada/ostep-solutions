// 6. Create a program that allocates an array of integers (as above), frees
// them, and then tries to print the value of one of the elements of
// the array. Does the program run? What happens when you use
// valgrind on it?

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *data = malloc(100 * sizeof(int));
    data[0] = 42;
    free(data);
    printf("%d\n", data[0]);
    return 0;
}

// gcc freed.c -o freed
// ./freed
// printed 34288042 (garbage) - NOT 42, confirming free() doesn't erase memory, it just marks it reusable; 
// the actual bytes are undefined/unreliable after freeing.

// valgrind --leak-check=yes ./freed
// Result: valgrind catches it - "Invalid read of size 4... Address is 0 bytes inside a block of size 400 free'd"
// confirming this was legitimately allocated memory that's since been freed. Interestingly, valgrind's own output showed the CORRECT value (42) rather than garbage - 
// valgrind's instrumentation changes memory behavior slightly, which itself demonstrates the core danger of use-after-free: behavior is unpredictable and inconsistent across runs/environments. You cannot rely on what you get back.
// this type of bug is called "use-after-free", and is worse than a leak or overflow because it doesn't fail consistently. 
// Sometimes it silently works by accident, sometimes it returns garbage, sometimes it crashes,
// depending on whether something else has reused that memory yet.