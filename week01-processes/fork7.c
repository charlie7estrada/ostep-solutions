// 7. Write a program that creates a child process, and then in the child
// closes standard output (STDOUT_FILENO). What happens if the child
// calls printf() to print some output after closing the descriptor?

#include <stdio.h>
#include <unistd.h>

int main()
{
    int rc = fork();

    if (rc == 0) {
        // child
        close(STDOUT_FILENO);
        printf("does this print?\n");
    } else {
        // parent
        printf("parent, pid = %d\n", getpid());
    }

    return 0;
}

// Closing STDOUT_FILENO in the child causes printf() calls after that point to silently fail - no output appears