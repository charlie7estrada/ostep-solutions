// 7. Now pass a funny value to free (e.g., a pointer in the middle of the
// array you allocated above). What happens? Do you need tools to
// find this type of problem?

#include <stdio.h>
#include <stdlib.h>

int main() {

    int *data = malloc(100 * sizeof(int));
    free(&data[50]);  // pointer to the middle of the array, not the start
    return 0;

}

// gcc badfree.c -o badfree
// ./badfree
// Result: Compiler caught it before runtime - a warning fired: "'free' called on pointer 'data' with nonzero offset 200 [-Wfree-nonheap-object]" 
// "free(): invalid pointer / Aborted (core dumped)" - the malloc library itself validates whatever pointer it's given against its internal allocation bookkeeping, and refuses to proceed when it doesn't match a legitimate allocation start address.
// we did not need any tools to catch this one. compiler gave us a warning
