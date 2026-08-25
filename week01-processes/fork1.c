// Questions
// 1. Write a program that calls fork(). Before calling fork(), have the
// main process access a variable (e.g., x) and set its value to something (e.g., 100). What value is the variable in the child process?
// What happens to the variable when both the child and parent change the value of x?

#include <stdio.h>
#include <unistd.h>

int main()
{
    int x = 100;
    int rc = fork();

    if (rc == 0) {
        // child
        printf("x = %d, pid = %d\n", x, getpid());
        x = 200;
        printf("x = %d, pid = %d\n", x, getpid());
    } else {
        // parent
        printf("x = %d, pid = %d\n", x, getpid());
        x = 300;
        printf("x = %d, pid = %d\n", x, getpid());
    }
    
    return 0;
}

// meaning the child and parent are independent, and when a variable changes in the parent the child is unaffected and vice versa