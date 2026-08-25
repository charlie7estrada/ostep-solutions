// 8. Try out some of the other interfaces to memory allocation. 
// For example, create a simple vector-like data structure and related routines that use realloc() to manage the vector. 
// Use an array to store the vectors elements; when a user adds an entry to the vector, use realloc() to allocate more space for it. 
// How well does such a vector perform? How does it compare to a linked list? 
// Use valgrind to help you find bugs

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *vec = NULL;      // starts empty
    int count = 0;        // how many elements currently in the vector

    for (int i = 0; i < 10; i++) {
        // grow the vector by one slot
        vec = realloc(vec, (count + 1) * sizeof(int));
        vec[count] = i;    // insert the new value
        count++;
    }

    // print everything to confirm it worked
    for (int i = 0; i < count; i++) {
        printf("%d ", vec[i]);
    }
    printf("\n");

    free(vec);
    return 0;
}

// gcc realloc.c -o realloc
// ./realloc
// valgrind --leak-check=yes ./realloc


// Built a naive vector using realloc() that grows by exactly 1 slot per insertion.
// printed 0-9 correctly
// valgrind shows no leaks/errors 

// Performance analysis: growing by exactly 1 element per insertion means insertion #n copies n-1 existing elements every time (realloc has to allocate new space AND copy all old data over). 
// Total copying work across N insertions grows roughly proportional to N² (quadratic) - so doubling the vector size roughly QUADRUPLES the total work, not just doubles it.

// Comparison to linked list: a linked list adds a new node with fixed, constant cost regardless of current size - no copying required, ever. For pure insertion performance, a naive grow-by-1 vector loses badly at scale.

// Fix: grow by DOUBLING capacity each time instead of by 1 (1->2->4->8->16...). Most insertions then have spare room already and need no realloc() at all; occasional reallocations still copy a lot, but happen rarely enough that average cost per insertion stays low. 
// This is the real strategy used by std::vector, Python lists, etc. Vectors still have a real tradeoff vs linked lists even with doubling: better for random access (O(1) index lookup) and cache locality (contiguous memory), worse for frequent insertions/deletions in the middle (requires shifting elements) - linked lists win there since insertion/deletion is just pointer rewiring.