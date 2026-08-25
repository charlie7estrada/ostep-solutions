// 1. First, write a simple program called null.c that creates a pointer
// to an integer, sets it to NULL, and then tries to dereference it. 
// Compile this into an executable called null. 
// What happens when you run this program?

#include <stdio.h>

int main(){

    int *ptr = NULL;
    printf("%d\n", *ptr);

}

// gcc null.c -o null
// ./null

// Result: Segmentation fault (core dumped). 
// I assumed dereferencing a NULL pointer would just return "nothing" or NULL itself, but that's wrong: *ptr is a direct instruction to go read memory at address 0 (which is what NULL actually is under the hood). 
// Address 0 is intentionally reserved/protected by the OS - accessing it triggers a protection violation (segfault), and the OS kills the process immediately. 

// ###################################################################################################################################

// 2. Next, compile this program with symbol information included (with the -g flag). 
// Doing so let’s put more information into the executable, enabling the debugger to access more 
// useful information about variable names and the like. 
// Run the program under the debugger by typing gdb ./null and then, once gdb is running,
// typing run. What does gdb show you?

// gcc null.c -o null -g
// gdb ./null
// run (then quit when done)

// Result:
// Program received signal SIGSEGV, Segmentation fault.
// 0x0000555555555161 in main () at null.c:11
// 11          printf("%d\n", *ptr);
// ^ this shows exactly which line and function contains a bug

// ###################################################################################################################################

// 3. Finally, use the valgrind tool on this program. We'll use memcheck
// that is a part of valgrind to analyze what happens. Run this by
// typing in the following: valgrind --leak-check=yes ./null.
// What happens when you run this? Can you interpret the output
// from the tool?

// valgrind --leak-check=yes ./null

// Result: valgrind caught "Invalid read of size 4" at null.c:11, and specifically noted "Address 0x0 is not stack'd, malloc'd or (recently) free'd" 
// explicitly confirming this memory was never legitimately allocated to the program. 
// HEAP SUMMARY shows 0 allocs/frees/leaks, since this bug has nothing to do with the heap - it's a raw invalid memory access, not a leak.

// Comparing tools: 
// gdb pinpoints WHERE a crash happens
// valgrind explains WHY an access is invalid and gives a broader memory health report (allocs/frees/leaks)