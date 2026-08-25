// 5. Write a program that creates an array of integers called data of size
// 100 using malloc; then, set data[100] to zero. What happens
// when you run this program? What happens when you run this
// program using valgrind? Is the program correct?

#include <stdio.h>
#include <stdlib.h>

int main() {
    int *data = malloc(100 * sizeof(int));
    data[100] = 0;
    printf("done\n");
    return 0;
}

// gcc data.c -o data
// ./data
// ran fine, no errors or crash

// valgrind --leak-check=yes ./data
// Result: valgrind catches some errors - "Invalid write of size 4... Address is 0 bytes after a block of size 400 alloc'd" - 
// confirming the write landed exactly 1 int past the end of the 400-byte (100-int) allocation. 
// Also caught a second, unintentional bug: "400 bytes definitely lost" - I forgot to free(data), so this program has both an overflow and a leak.